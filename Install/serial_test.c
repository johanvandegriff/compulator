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
  int fd;

  //try to open the serial port
  if((fd = serialOpen ("/dev/serial0", 9600)) < 0 ){
    perror("/dev/serial0 could not be opened.\n");
  }
  if ( wiringPiSetup () < 0 ){
    perror("WiringPiSetup problem \n ");
  }

  printf("Setting screen brightness...\n");
  int i;
  for(i=0; i<256; i++){
    serialPutchar(fd, SET_SCREEN_BRIGHTNESS);
    serialPutchar(fd, i);
    delay(20);
  }

  printf("Turning screen off...\n");
  serialPutchar(fd, SET_SCREEN_OFF);
  delay(1000);
  printf("Turning screen on...\n");
  serialPutchar(fd, SET_SCREEN_ON);
  delay(1000);

  printf("Requesting CPU Temperature...");
  serialPutchar(fd, GET_CPU_TEMP);
  printf("%d\n",serialGetchar(fd));

  /*delay(1000);
  serialPutchar(fd, TURN_OFF_POWER);
  int shutdownDelay = 10000; //10 seconds

  serialPutchar(fd, (int) shutdownDelay / 256);
  serialPutchar(fd, shutdownDelay % 256);*/

  return 0;
}
