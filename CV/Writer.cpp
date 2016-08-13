#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
using namespace std;

int main()
{
    int fd;
    char * myfifo = "/tmp/myfifo";
    cout << "success1" << endl;
    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);
    cout << "success2" << endl;
    /* write "Hi" to the FIFO */


    fd = open(myfifo, O_WRONLY);
    for(int i=0;i<10;i++){
        cout << i<<endl;
        char str = 'a';//to_string(double(i));
        cout << str << endl;
        write(fd, str, sizeof(str));
    }
    close(fd);

    cout << "success final" << endl;
    /* remove the FIFO */
    unlink(myfifo);

    return 0;
}
