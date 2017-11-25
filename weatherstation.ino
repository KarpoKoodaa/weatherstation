/*
	Weatherstation
	Arduino weatherstation that uses DHT11 sensor to log and send data to InfluxDB over Wi-Fi

	by Kariantti Laitala

	Start date: 2017-11-07
*/

#define ESP8266 Serial1
#include "DHT.h"
#define DHTTYPE DHT11 //DHT type is DHT11
#define DHTPIN 2  //DHT11 connected to DPIN2
DHT dht(DHTPIN,DHTTYPE);

String SSID = "wifi SSID";
String PASS = "Wifi password";

boolean FAIL8266 = false; //Fail check of ESP8266 module

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

void setup(){

	Serial.begin(115200); //Debuggin console
	ESP8266.begin(115200); //Connectivity to ESP8266

	while(!Serial);

	Serial.println("--- Start ---");
	sendESP8266Cmdln("AT+RST",500);

	//WONT WORK NEED TO CHECK
	// Read input from ESP and parse it and check what is the status
	/*if (ESP8266.find("DISCONNECTED"))
	{
		Serial.println("Wi-Fi DISCONNECTED");
		cwJoinAP();
	}

	if (ESP8266.find("CONNECTED"))
	{
		Serial.println("Wi-Fi connected. IP address:");
		sendESP8266Cmdln("AT+CIFSR", 1000);
	}*/


}

void loop()
{
	String TARGET_TYPE ="TCP";
	String TARGET_ADDRESS = "192.168.1.XXX";
	String TARGET_PORT = "8086";

	int temperature;
	int humidity;

	//Wait to until both has a value
	do{
		temperature = dht.readTemperature();
		humidity = dht.readHumidity();
	}while(isnan(temperature) || isnan(humidity));

	//Print values to console
	Serial.println("Temperature: "+ String(temperature));
	Serial.println("Humidity: "+ String(humidity));

	//Start connection to DB
	//CIPSTART command
	String CIPSTART = "AT+CIPSTART=";
	CIPSTART += "\"" + TARGET_TYPE + "\",\"" + TARGET_ADDRESS + "\",";
	CIPSTART += TARGET_PORT;

	Serial.println(CIPSTART); //To check CIPSTART command. Can be commented out

	// Send command to ESP
	ESP8266.println(CIPSTART);
	showESP8266SerialBuffer();

	//prepare data for DB
	String db_content = CreateDataDB("temperature", temperature, TARGET_ADDRESS);

	delay(2000);

	//Send data to DB
	String cmdSendLength = "AT+CIPSEND=";
	cmdSendLength += String(db_content.length());
	Serial.println(cmdSendLength);
	ESP8266.println(cmdSendLength);

	//Print ESPP console data to console
	showESP8266SerialBuffer();

	//Wait until ESP is ready to send data
	Serial.println("Waiting >");
	if (ESP8266.find(">"))
	{
		Serial.println("> received");
		ESP8266.println(db_content);
		Serial.println(db_content);

		boolean OK_FOUND = false;
	}
	//Block program until  OK found



	


	
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

String CreateDataDB(String datatype, int data, String TARGET_ADDRESS)
{
	String InfluxDBMethod = "POST /write?db=db_TempHum&precision=s HTTP/1.1\r\n";
  	String InfluxDBUserAgent = "User-Agent: Arduino/1.1\r\n";
  	String InfluxDBHost = "Host: "+ TARGET_ADDRESS + "\r\n";
  	String InfluxDBConnection = "Accept: */*\r\n";
  	String InfluxDBConnectionType = "Connection: close\r\n";
  	String InfluxDBContentType = "Content-Type: application/json\r\n";
  	String content = "DHT11,sensor=Test" + datatype + "="+String(data);
	String InfluxDBContentLength = "Content-Length:"+ String(content.length()) +"\r\n\n";

	String HTTP_RQS = InfluxDBMethod;
    HTTP_RQS += InfluxDBUserAgent;
  	HTTP_RQS += InfluxDBHost;
  	HTTP_RQS += InfluxDBConnection;
    HTTP_RQS += InfluxDBConnectionType;
  	HTTP_RQS += InfluxDBContentType;
  	HTTP_RQS += InfluxDBContentLength;
  	HTTP_RQS += content;

  	Serial.println(HTTP_RQS);
  	return(HTTP_RQS);

}


