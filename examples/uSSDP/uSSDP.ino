/**
 * SSDP Device example
 * Process SSDP request and NOTIFY services and presence
 * 
 * This example code is in the public domain.
 **/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUDP.h>
#include <uSSDP.h>

const char* ssid = "********";
const char* password = "********";

ESP8266WebServer HTTP(80);
uDevice device;
uSSDP SSDP;

void setup(){
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if(WiFi.waitForConnectResult() != WL_CONNECTED){
      Serial.println("WiFi Failed");
      while(1) delay(100);
  }
  
	Serial.print("Starting HTTP at ");
	Serial.print(WiFi.localIP());
	Serial.print(":");
	Serial.println(80);
  
  HTTP.on("/ssdp/schema.xml", HTTP_GET, [](){
    WiFiClient client = HTTP.client();
    SSDP.schema(&client);
    client.stop();
  });
	HTTP.begin();
	
  byte mac[6];
  char base[UUIDBASE_SIZE];
  WiFi.macAddress(mac);
  sprintf(base, "esp8266x-%02x%02x-%02x%02x-%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.printf("Starting uSSDP: BASE: %s, MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", base, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  device.begin((const char*)base, mac);
  device.serialNumber((char*)"A0123456789");
  device.manufacturer((char*)"Espressif");
  device.manufacturerURL((char*)"http://espressif.com");
  device.modelName((char*)"ESP-12e");
  device.modelNumber(1, 0);
  device.friendlyName((char*)"ESP8266");
  device.presentationURL((char*)"/");
	SSDP.begin(&device);
  Serial.println("SSDP Started");
}

void loop(){
  HTTP.handleClient();
	SSDP.process();
  delay(1);
}
