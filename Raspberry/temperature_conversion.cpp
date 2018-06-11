#include <cstdio>
#include <cstring>

#include "temperature_conversion.h"
#include <mosquittopp.h>

mqtt_tempconv::mqtt_tempconv(const char *id, const char *host, int port,char*message) : mosquittopp(id)
{
	int keepalive = 10;

	/* Connect immediately. This could also be done by calling
	 * mqtt_tempconv->connect(). */
	connect(host, port, keepalive);
	publish(NULL, "/avocarbon/test", strlen(message), message);
	this->disconnect();
};

mqtt_tempconv::~mqtt_tempconv()
{
}

void mqtt_tempconv::on_connect(int rc)
{
	printf("Connected with code %d.\n", rc);
	if(rc == 0){
		/* Only attempt to subscribe on a successful connect. */
		subscribe(NULL, "/avocarbon/test");
	}
}

void mqtt_tempconv::on_message(const struct mosquitto_message *message)
{
	
	char* buf = new char[300];
	formatage(buf);
	publish(NULL, "/avocarbon/test", strlen(buf), buf);
	this->disconnect();
}

void mqtt_tempconv::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	printf("Subscription succeeded.\n");
}
void mqtt_tempconv::formatage(char* strr)
{
  
  char tmp[10];
  sprintf(tmp, "%d", 10 );
  strcpy(strr, "{ \"date\":\"\", \"nbPieceBonne\":\"");
  strcat(strr, tmp); strcat(strr, "\",\"Mac\":\"");
  strcat(strr, "00:00");
  strcat(strr, "\",\"nbPieceMauvaise\":\"\",\"trame\":{\"entry0\":\""); sprintf(tmp, "%d", 0 ); strcat(strr, tmp);
  strcat(strr, "\",\"input1\":\""); sprintf(tmp, "%d", 0 ); strcat(strr, tmp);
  strcat(strr, "\",\"input2\":\""); sprintf(tmp, "%d", 0); strcat(strr, tmp);
  strcat(strr, "\",\"input3\":\""); sprintf(tmp, "%d", 0 ); strcat(strr, tmp);
  strcat(strr, "\",\"input4\":\""); sprintf(tmp, "%d", 0 ); strcat(strr, tmp);
  strcat(strr, "\",\"input5\":\""); sprintf(tmp, "%d", 0 ); strcat(strr, tmp);
  strcat(strr, "\",\"input6\":\""); sprintf(tmp, "%d", 0 ); strcat(strr, tmp);
  strcat(strr, "\",\"input7\":\""); sprintf(tmp, "%d", 0 ); strcat(strr, tmp);
  strcat(strr, "\"}} ");
  //return strr;

}
