#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(){
    
    string old_color = "red";
    double old_dist = 0;
    double old_angle = 0;
    string color;
    double dist;
    double angle;
    
    while (1){
        ifstream fread;
        fread.open("para.txt");
        fread>> color;
        fread >> dist;
        fread >> angle;
        // check if there exists updated paras
        if(color!=old_color || dist!=old_dist || angle!=old_angle){
             cout<<"color => "<<color<<endl;
             cout<<"dist : "<<dist<<endl;
             cout<<"angle : "<<angle<<endl;
             // update old paras
             old_color = color;
             old_dist = dist;
             old_angle = angle;
             
        }
        fread.close();
    }

    
    
    
    system("PAUSE");
    return 0;
}
