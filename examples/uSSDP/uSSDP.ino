/**
 * SSDP Service
 * 
 * This example code is in the public domain.
 **/
#include <SPI.h>
#include <Ethernet.h>
#include <uHTTP.h>
#include <uSSDP.h>

#define DEBUG 1

uint8_t macaddr[6] = { 0x74,0x99,0x69,0x2D,0x30,0x40 };
IPAddress ipaddr(192, 168, 0, 254);
IPAddress gateway(192, 168, 0, 1);

uHTTP *HTTP;
uSSDP *SSDP;

SSDP_DEVICE device;
EthernetClient *response;

unsigned long _time = 0;

void setup(){
	#if DEBUG > 0
	Serial.begin(9600);
	#endif

	Ethernet.begin(macaddr, ipaddr, gateway, gateway);

	#if DEBUG > 0
	Serial.print(F("Starting uHTTP at "));
	Serial.println(ipaddr);
	#endif
	HTTP = new uHTTP(80);

	#if DEBUG > 0
	Serial.println(F("Starting uSSDP..."));
	#endif

	strcpy_P(device.name, PSTR("DOMO"));
	strcpy_P(device.uuid, PSTR("abcdefgh-7dec-11d0-a765-7499692d3040"));
	strcpy_P(device.addr, PSTR("192.168.0.254"));
	SSDP = new uSSDP(device);
}

void loop(){

	// Process SSDP request
	if(SSDP->process()){
		SSDP_HEADER head = SSDP->head();

		if(SSDP->method() == METHOD_MSEARCH){
			//Serial.println(F("Method: M-SEARCH"));
			SSDP->send(SSDP_RESPONSE);
		}
	}

	if((millis() - _time) > (SSDP_INTERVAL * 1000)){
		_time = millis();
		SSDP->send(SSDP_NOTIFY);
	}

	// Process HTTP request
	if((response = HTTP->process())){
		if(strcmp_P(HTTP->uri(), PSTR("/ssdp/schema.xml")) == 0){
			schema(response);
		}else{
			response->println(F("HTTP/1.1 200 OK"));
			response->println(F("Content-Type: text/plain"));
			response->println();
			response->println(F("Hello World!"));
		}
		response->stop();
	}

	delay(10);
}

void schema(EthernetClient *response){
  	response->println(F("HTTP/1.1 200 OK"));
	response->println(F("Content-Type: text/xml"));
	response->println();
	response->println(F("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
	response->println(F("\t<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"));
	response->println(F("\t\t<specVersion>"));
	response->println(F("\t\t\t<major>1</major>"));
	response->println(F("\t\t\t<minor>0</minor>"));
	response->println(F("\t\t</specVersion>"));
	response->println(F("\t\t<device>"));
	response->println(F("\t\t\t<deviceType>urn:schemas-upnp-org:device:BinaryLight:1</deviceType>"));
	response->println(F("\t\t\t<presentationURL>/</presentationURL>"));
	response->println("\t\t\t<friendlyName>" + String(device.name) + "</friendlyName>");	
	response->println(F("\t\t\t<manufacturer>Nomad NT</manufacturer>"));
	response->println(F("\t\t\t<manufacturerURL>http://nomadnt.com</manufacturerURL>"));
	response->println("\t\t\t<modelName>" + String(device.name) + "</modelName>");
	response->println(F("\t\t\t<serialNumber>1</serialNumber>"));
	response->println("\t\t\t<UDN>uuid:" + String(device.uuid) + "</UDN>");
	response->println(F("\t\t</device>"));
	response->println(F("\t</root>"));
}