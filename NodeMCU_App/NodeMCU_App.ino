/**************************************************************************************************
 * NodeMCU_App.ino
 *
 * Created: 18/05/2020
 * Author: Lars Lindner
 *
***************************************************************************************************/

#define D0		16
#define D1		5
#define D2		4
#define D3		0
#define led_pin	2
#define D5		14
#define D6		12
#define D7		13
#define D8		15
#define RX		3
#define TX		1


#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
//#include <WiFiClient.h>
#include<SoftwareSerial.h>

const char* ssid = "INFINITUM7271_2.4";
const char* password = "MODEM08042020";

WiFiServer server(80);
SoftwareSerial mySerial(D1,D2);


void setup() 
{
	Serial.begin(115200);				//Hardware Serial with default Baud Rate for NodeMCU
	delay(10);
	mySerial.begin(9600);				//Software Serial for communication with Arduino
	delay(10);
	
	pinMode(led_pin, OUTPUT);
	
	// Connect to WiFi network
	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);
	
	WiFi.begin(ssid, password);
	IPAddress ip(192,168,1,67);
	IPAddress gateway(192,168,1,254);
	IPAddress subnet(255,255,255,0);
	WiFi.config(ip, gateway, subnet);	
	
	while (WiFi.status() != WL_CONNECTED) 
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	
	// Start the server
	server.begin();
	Serial.println("Server started");
	
	// Print the IP address
	Serial.println(WiFi.localIP());

	// Signal for user that NodeMCU starts with the loop()
	for (int i = 0; i < 4; i++)
	{
		digitalWrite(led_pin, LOW);			// LOW on the NodeMCU means ON
		delay(250);
		digitalWrite(led_pin, HIGH);		// High on the NodeMCU means OFF
		delay(250);
	}
}


void loop() 
{
	WiFiClient client = server.available();
	if (!client) 
	{
		return;
	}
	
	// Wait until the client sends some data
	Serial.println("new client");
	while(!client.available())
	{
		delay(1);
	}
	
	// The complete string from the http request is read
	String cmpl_str = client.readStringUntil('\r');

// 	// Insert a delay, so that serial data isn't send too fast, to avoid flickering of lamp
// 	delay(100);
			
	// The command and the value is extracted from the http string
	String tmp_str = cmpl_str.substring(cmpl_str.indexOf("/") + 1);
	tmp_str.remove(tmp_str.indexOf(" "));
	String cmd_str = tmp_str;
	cmd_str.remove(cmd_str.indexOf("/"));
	int cmd_int = tmp_str.substring(tmp_str.indexOf("/") + 1).toInt();

	// and send to the Arduino
 	mySerial.print(cmd_str);
	mySerial.print("_");
 	mySerial.println(cmd_int, DEC);

	// HTTP response
	client.flush();	
	String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nData Received ";
	s +=  tmp_str;
	s += " </html>";
	client.println(s);

}






