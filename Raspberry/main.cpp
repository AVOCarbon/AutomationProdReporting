#include <cstdio>
#include <cstring>
#include "temperature_conversion.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <ctime>
#include <algorithm>
bool InitTrame [8];
uint8_t StatusTrame [8];
int PieceBonne = 0;
int PieceMauvaise = 0;
int tampon = -1;
int tmp;
char* strr = new char[300];
char* MacAdress ;
class mqtt_tempconv *tempconv;
void formatage(char* strr)
{
  //get current time in format of time_t
  time_t t = time(NULL);
  //show the value stored in t
  //convert time_t to char*
  std::string charTimetmp = ctime(&t);
  int sizedate = charTimetmp.length();
  charTimetmp.erase(std::remove(charTimetmp.begin(), charTimetmp.end(), '\n'), charTimetmp.end());
  const char * charTime = charTimetmp.c_str();
  StatusTrame[tmp] = !InitTrame[tmp] ;
  char tmp[10];
  sprintf(tmp, "%d", PieceBonne );
  strcpy(strr, "{ \"date\":\"");
  strcat(strr, charTime);
  strcat(strr, "\", \"nbPieceBonne\":\"");
  strcat(strr, tmp); strcat(strr, "\",\"Mac\":\"");
  strcat(strr, MacAdress);
  strcat(strr, "\",\"nbPieceMauvaise\":\"\",\"trame\":{\"entry0\":\""); sprintf(tmp, "%d", StatusTrame[0] ); strcat(strr, tmp);
  strcat(strr, "\",\"input1\":\""); sprintf(tmp, "%d", StatusTrame[1] ); strcat(strr, tmp);
  strcat(strr, "\",\"input2\":\""); sprintf(tmp, "%d", StatusTrame[2] ); strcat(strr, tmp);
  strcat(strr, "\",\"input3\":\""); sprintf(tmp, "%d", StatusTrame[3] ); strcat(strr, tmp);
  strcat(strr, "\",\"input4\":\""); sprintf(tmp, "%d", StatusTrame[4] ); strcat(strr, tmp);
  strcat(strr, "\",\"input5\":\""); sprintf(tmp, "%d", StatusTrame[5] ); strcat(strr, tmp);
  strcat(strr, "\",\"input6\":\""); sprintf(tmp, "%d", StatusTrame[6] ); strcat(strr, tmp);
  strcat(strr, "\",\"input7\":\""); sprintf(tmp, "%d", StatusTrame[7] ); strcat(strr, tmp);
  strcat(strr, "\"}} ");

}
void SendMessage(){
	formatage(strr);
	PieceBonne = 0;
	tempconv = new mqtt_tempconv("avocarbon", "192.168.79.13", 1883,strr);
	tempconv->loop_forever();
}
int initTrame()
{
  int i = 0;
  int fail = -1;
  while (i < 8)
  {
    if (InitTrame[i] != StatusTrame[i])
    {
      fail = i;
    }
    i++  ;
  }
  return fail;
}
int main(int argc, char *argv[])
{
	printf("start");
	
	//int rc;
	
  FILE * pFile;
  long lSize;
  size_t result;
  pFile = fopen ( "/sys/class/net/wlan0/address", "r" );
  if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = 20;
  rewind (pFile);

  // allocate memory to contain the whole file:
  MacAdress =  new char[sizeof(char)*lSize];
  printf("MacAdress = %d\n",sizeof(char)*lSize);
  if (MacAdress == NULL) {fputs ("Memory error",stderr); exit (2);}
  printf("MacAdress = %p\n", MacAdress);
  // copy the file into the buffer:
  result = fread (MacAdress,1,lSize-3,pFile);printf("MacAdress = %p\n", MacAdress);
  //if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
  /* the whole file is now loaded in the memory buffer. */

  fclose (pFile);
 
  InitTrame[0] = 1;
  InitTrame[1] = 1;
  InitTrame[2] = 0;
  InitTrame[3] = 0;
  InitTrame[4] = 0;
  InitTrame[5] = 0;
  InitTrame[6] = 0;
  InitTrame[7] = 0;
  
	mosqpp::lib_init();
	mosqpp::lib_cleanup();

  int fd, n;
  char buf[64] = "temp text";
  struct termios toptions;

  /* open serial port */
  fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
  printf("fd opened as %i\n", fd);
  
  /* wait for the Arduino to reboot */
  //usleep(3500000);
  
  /* get current serial port settings */
  tcgetattr(fd, &toptions);
  /* set 9600 baud both ways */
  cfsetispeed(&toptions, B9600);
  cfsetospeed(&toptions, B9600);
  /* 8 bits, no parity, no stop bits */
  toptions.c_cflag &= ~PARENB;
  toptions.c_cflag &= ~CSTOPB;
  toptions.c_cflag &= ~CSIZE;
  toptions.c_cflag |= CS8;
  /* Canonical mode */
  toptions.c_lflag |= ICANON;
  /* commit the serial port settings */
  tcsetattr(fd, TCSANOW, &toptions);

  /* Send byte to trigger Arduino to send string back */
  write(fd, "0", 1);
  /* Receive string from Arduino */
  while(1)
  {
  n = read(fd, buf, 64);
  /* insert terminating zero in the string */
  buf[n] = 0;

  	if(n>1)
  	{		
		  tmp = (int)buf[0]-48;
		
		  if (tmp != tampon)
		  {
			printf("%i bytes read, buffer contains: %s\n", n, buf);
			printf("%i bytes read, buffer contains: %d\n", n, tmp);
			StatusTrame[0] = InitTrame[0] ;
			StatusTrame[1] = InitTrame[1] ;
			StatusTrame[2] = InitTrame[2] ;
			StatusTrame[3] = InitTrame[3] ;
			StatusTrame[4] = InitTrame[4] ;
			StatusTrame[5] = InitTrame[5] ;
			StatusTrame[6] = InitTrame[6] ;
			StatusTrame[7] = InitTrame[7] ;
			
			switch ( tmp )
			{
			  case 0:
				printf("Work"); tampon = 0; SendMessage(); break;
			  case 1:
				printf("Start"); tampon = 1; SendMessage(); break;
			  case 2:
				printf("PieceBonne : "); PieceBonne++; tampon = 2; break;//Serial.print(PieceBonne); Serial.print( "hours :" ); Serial.print(hour()) ; Serial.print("minutes :" ); Serial.print(minute()); Serial.print("days :"); Serial.print(day());
			  case 3:
				printf("PieceMauvaise : "); PieceMauvaise++; tampon = 3; printf("%d\n", PieceMauvaise); break;
			  case 4:
				printf("missPm1"); tampon = 4; SendMessage(); break;
			  case 5:
				printf("missPm2"); tampon = 5; SendMessage(); break;
			  case 6:
				printf("Maintenance"); tampon = 6; SendMessage(); break;
			  case 7:
				printf("Stop");  tampon = 7; SendMessage(); break;
			  default :
				printf("Production"); tampon = -1; break;
			}
			if (PieceBonne >= 10)
			  {
				
				SendMessage();
				
			  }
		  }
		
		
	}
  }
 
  return 0;
}

