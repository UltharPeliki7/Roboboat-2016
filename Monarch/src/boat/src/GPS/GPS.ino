/*
 * uBlox UBX Protocol Reader (runs on Arduino Leonardo, or equivalent)
 *
 * Note: RX pad on 3DR Module is output, TX is input
 */
#include <PString.h>
 
#include <ros.h>
#include <ros/time.h>
#include <std_msgs/Float64.h>
#include <std_msgs/String.h> 
#define MAX_LENGTH 512

#define  POSLLH_MSG  0x02
#define  SBAS_MSG    0x32
#define  VELNED_MSG  0x12
#define  STATUS_MSG  0x03
#define  SOL_MSG     0x06
#define  DOP_MSG     0x04
#define  DGPS_MSG    0x31

#define LONG(X)    *(long*)(&data[X])
#define ULONG(X)   *(unsigned long*)(&data[X])
#define INT(X)     *(int*)(&data[X])
#define UINT(X)    *(unsigned int*)(&data[X])

ros::NodeHandle nh;

std_msgs::String gps;
std_msgs::Float64 head;


ros::Publisher GPS_Lat("Latitude", &gps);
ros::Publisher GPS_Lon("Longitude", &gps);
ros::Publisher CH("Heading", &head);

unsigned char  state, lstate, code, id, chk1, chk2, ck1, ck2;
unsigned int  length, idx, cnt;

unsigned char data[MAX_LENGTH];

long lastTime = 0;

void enableMsg (unsigned char id, boolean enable) {
  //               MSG   NAV   < length >  NAV
  byte cmdBuf[] = {0x06, 0x01, 0x03, 0x00, 0x01, id, enable ? 1 : 0};
  sendCmd(sizeof(cmdBuf), cmdBuf);
}

void setup() {
  nh.initNode();
  
  nh.advertise(GPS_Lat);
  nh.advertise(GPS_Lon);
  nh.advertise(CH);
  
  Serial.begin(57600);
  Serial1.begin(38400);
  if (false) {
    while(Serial.available() == 0)
      ;
    lstate = state = 0;
  }
  // Modify these to control which messages are sent from module
  enableMsg(POSLLH_MSG, true);    // Enable position messages
  enableMsg(SBAS_MSG, false);      // Enable SBAS messages
  enableMsg(VELNED_MSG, true);    // Enable velocity messages
  enableMsg(STATUS_MSG, false);    // Enable status messages
  enableMsg(SOL_MSG, false);       // Enable soluton messages
  enableMsg(DOP_MSG, false);       // Enable DOP messages
  enableMsg(DGPS_MSG, false);     // Disable DGPS messages
}

void loop() {
  if (Serial1.available()) {
    unsigned char cc = Serial1.read();
    switch (state) {
      case 0:    // wait for sync 1 (0xB5)
        ck1 = ck2 = 0;
        if (cc == 0xB5)
          state++;
        break;
      case 1:    // wait for sync 2 (0x62)
        if (cc == 0x62)
          state++;
        else
          state = 0;
        break;
      case 2:    // wait for class code
        code = cc;
        ck1 += cc;
        ck2 += ck1;
        state++;
        break;
      case 3:    // wait for Id
        id = cc;
        ck1 += cc;
        ck2 += ck1;
        state++;
        break;
      case 4:    // wait for length byte 1
        length = cc;
        ck1 += cc;
        ck2 += ck1;
        state++;
        break;
      case 5:    // wait for length byte 2
        length |= (unsigned int) cc << 8;
        ck1 += cc;
        ck2 += ck1;
        idx = 0;
        state++;
        if (length > MAX_LENGTH)
          state= 0;
        break;
      case 6:    // wait for <length> payload bytes
        data[idx++] = cc;
        ck1 += cc;
        ck2 += ck1;
        if (idx >= length) {
          state++;
        }
        break;
      case 7:    // wait for checksum 1
        chk1 = cc;
        state++;
        break;
      case 8:    // wait for checksum 2
        chk2 = cc;
        boolean checkOk = ck1 == chk1  &&  ck2 == chk2;
        if (checkOk) {
          switch (code) {
            case 0x01:      // NAV-
              // Add blank line between time groups
              /*if (lastTime != ULONG(0)) {
                lastTime = ULONG(0);
                Serial.print(F("\nTime: "));
                Serial.println(ULONG(0), DEC);
              }
              Serial.print("NAV-");
              */
              switch (id) {
                case 0x02:  // NAV-POSLLH
                
                Serial.print(F("latitude = "));            //Latitude
                Serial.println(printLat(LONG(8)));
                char charLAT[11]; 
                (printLat(LONG(8))).toCharArray(charLAT, 11);
                 gps.data = charLAT;              
             //    Serial.println(gps.data);
                 GPS_Lat.publish( &gps );
               
                 Serial.println(" ");
                  Serial.print(F("longitude = "));      //Longitude
                  Serial.println(printLon(LONG(4)));
                  char charLON[11]; 
                  (printLon(LONG(4))).toCharArray(charLON, 11);
                  gps.data = charLON;              
                // Serial.println(gps.data);
                  GPS_Lon.publish( &gps );
                 
                /*  Serial.print(F("POSLLH: lon = "));
                  printLatLon(LONG(4));
                  Serial.print(F(", lat = "));
                  printLatLon(LONG(8));
                  Serial.print(F(", vAcc = "));
                  Serial.print(ULONG(24), DEC);
                  Serial.print(F(" mm, hAcc = "));
                  Serial.print(ULONG(20), DEC);
                  Serial.print(F(" mm"));
                  */
                 break;
                case 0x03:  // NAV-STATUS
                  Serial.print(F("STATUS: gpsFix = "));
                  Serial.print(data[4], DEC);
                  if (data[5] & 2) {
                    Serial.print(F(", dgpsFix"));
                  }
                  break;   
               case 0x04:  // NAV-DOP
                  Serial.print(F("DOP:    gDOP = "));
                  Serial.print((float) UINT(4) / 100, 2);
                  Serial.print(F(", tDOP = "));
                  Serial.print((float) UINT(8) / 100, 2);
                  Serial.print(F(", vDOP = "));
                  Serial.print((float) UINT(10) / 100, 2);
                  Serial.print(F(", hDOP = "));
                  Serial.print((float) UINT(12) / 100, 2);
                  break;
                case 0x06:  // NAV-SOL
                  Serial.print(F("SOL:    week = "));
                  Serial.print(UINT(8), DEC);
                  Serial.print(F(", gpsFix = "));
                  Serial.print(data[10], DEC);
                  Serial.print(F(", pDOP = "));
                  Serial.print((float) UINT(44) / 100.0, 2);
                  Serial.print(F(", pAcc = "));
                  Serial.print(ULONG(24), DEC);
                  Serial.print(F(" cm, numSV = "));
                  Serial.print(data[47], DEC);
                  break;
                case 0x12:  // NAV-VELNED
                /*  Serial.print(F("VELNED: gSpeed = "));
                  Serial.print(ULONG(20), DEC);
                  Serial.print(F(" cm/sec, sAcc = "));
                  Serial.print(ULONG(28), DEC);
                  */
                  Serial.print(F("heading = "));
                  Serial.print((float) LONG(24) / 100000, 2);
                  Serial.print(F(" deg"));
                  Serial.println(" ");
                 
                  /*
                  Serial.print((float) LONG(32) / 100000, 2);
                  Serial.print(F(" deg"));
                  */
                  break;
                case 0x31:  // NAV-DGPS
                  Serial.print(F("DGPS:   age = "));
                  Serial.print(LONG(4), DEC);
                  Serial.print(F(", baseId = "));
                  Serial.print(INT(8), DEC);
                  Serial.print(F(", numCh = "));
                  Serial.print(INT(12), DEC);
                  break;
                case 0x32:  // NAV-SBAS
                  Serial.print(F("SBAS:   geo = "));
                  switch (data[4]) {
                    case 133:
                      Serial.print(F("Inmarsat 4F3"));
                      break;
                    case 135:
                      Serial.print(F("Galaxy 15"));
                      break;
                    case 138:
                      Serial.print(F("Anik F1R"));
                      break;
                    default:
                      Serial.print(data[4], DEC);
                      break;
                  }
                  Serial.print(F(", mode = "));
                  switch (data[5]) {
                    case 0:
                      Serial.print(F("disabled"));
                      break;
                    case 1:
                      Serial.print(F("enabled integrity"));
                      break;
                    case 2:
                      Serial.print(F("enabled test mode"));
                      break;
                    default:
                      Serial.print(data[5], DEC);
                  }
                  Serial.print(F(", sys = "));
                   switch (data[6]) {
                    case 0:
                      Serial.print(F("WAAS"));
                      break;
                    case 1:
                      Serial.print(F("EGNOS"));
                      break;
                    case 2:
                      Serial.print(F("MSAS"));
                      break;
                     case 16:
                      Serial.print(F("GPS"));
                      break;
                   default:
                      Serial.print(data[6], DEC);
                  }
                  break;
                default:
                  printHex(id);
              }
              Serial.println();
              break;
            case 0x05:      // ACK-
              Serial.print(F("ACK-"));
              switch (id) {
                case 0x00:  // ACK-NAK
                Serial.print(F("NAK: "));
                break;
                case 0x01:  // ACK-ACK
                Serial.print(F("ACK: "));
                break;
              }
              printHex(data[0]);
              Serial.print(" ");
              printHex(data[1]);
              Serial.println();
              break;
          }
        }
        state = 0;
        break;
    }
  }
  nh.spinOnce();
}

  // Convert 1e-7 value packed into long into decimal format
  String printLat (long val) {
 
  char buffer[14];
  PString str(buffer, sizeof(buffer));
  str.print(val, DEC);
  String lat_s(buffer); 
  return lat_s; 

  }

  String printLon (long val) 
  {
  char buffer[14];
  PString str(buffer, sizeof(buffer));
  str.print(val, DEC);
  String lon_s(buffer);
  return lon_s;
  }
  
void printHex (unsigned char val) {
  if (val < 0x10)
    Serial.print("0");
  Serial.print(val, HEX);
}

void sendCmd (unsigned char len, byte data[]) {
  Serial1.write(0xB5);
  Serial1.write(0x62);
  unsigned char chk1 = 0, chk2 = 0;
  for (unsigned char ii = 0; ii < len; ii++) {
    unsigned char cc = data[ii];
    Serial1.write(cc);
    chk1 += cc;
    chk2 += chk1;
  }
  Serial1.write(chk1);
  Serial1.write(chk2);
}

