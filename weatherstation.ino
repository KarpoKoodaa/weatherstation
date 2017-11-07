/*
	Weatherstation
	Arduino weatherstation that uses DHT11 sensor to log and send data to InfluxDB over Wi-Fi

	by Kariantti Laitala

	Start date: 2017-11-07
*/

#define ESP8266 Serial1

#include "dht.h"
#define DHTTYPE DHT11 //DHT type is DHT11
#define DHTPIN 2  //DHT11 connected to DPIN2
DHT dht(DHTTYPE,DHTPIN);

String SSID = "wifi SSID";
String PASS = "Wifi password";

boolean FAIL8266 = false; //Fail check of ESP8266 module

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

void setup()
{
	Serial.begin(115200); //Debuggin console
	ESP8266.begin(115200); //Connectivity to ESP8266

	while(!Serial);

	Serial.println("--- Start ---");





}

void loop()
{
	
}

//Send command to ESP8266 console
void sendESP8266Cmdln(String cmd,int waitTime){
	ESP8266.println(cmd);
	delay(waitTime);
	showESP8266SerialBuffer();

}

//Prints ESP8266 serial buffer to console
void showESP8266SerialBuffer(){

	Serial.println("=== ESP8266 Serial ====");
	while (ESP8266.available() > 0){
		char a = ESP8266.read();
		Serial.write(a);

	}
	Serial.println("\n=== ESP8266 message ends ====");
}
boolean cwJoinAP()  // Join Wi-Fi network
{
	String cmd="AT+CWJAP=\"";
	cmd+=SSID;
	cmd+="\",\"";
	cmd+=PASS;
	cmd+="\"";
	ESP8266.println(cmd);
	Serial.println(cmd);
	delay(1000);
}
