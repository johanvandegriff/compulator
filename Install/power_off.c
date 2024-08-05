#include <wiringPi.h>
#include <string.h>
//#include <errno.h>
//#include <sys/types.h>
#include <wiringSerial.h>
#include <stdio.h>


//command definitions
#define SET_SCREEN_OFF 10
#define SET_SCREEN_ON 11
#define GET_SCREEN_STATE 12

#define SET_SCREEN_BRIGHTNESS 15
#define GET_SCREEN_BRIGHTNESS 16

#define GET_CPU_TEMP 30

#define TURN_OFF_POWER 40

#define GO_TO_SLEEP 101
#define WAKE_UP 102

int main(int argc, char** argv){
  if(argc > 1 && !strcmp("reboot", argv[1])){
    return 0;
  }

  int fd;

  //try to open the serial port
  if((fd = serialOpen ("/dev/serial0", 9600)) < 0 ){
    perror("device not opened \n");
  }
  if ( wiringPiSetup () < 0 ){
    perror("WiringPiSetup problem \n ");
  }

  serialPutchar(fd, TURN_OFF_POWER);
  int shutdownDelay = 3000; //3 seconds

  serialPutchar(fd, (int) shutdownDelay / 256);
  serialPutchar(fd, shutdownDelay % 256);

  return 0;
}
