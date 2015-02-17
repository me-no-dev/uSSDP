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
IPAddress ipaddr(192, 168, 10, 254);
IPAddress gateway(192, 168, 10, 1);

uHTTP *HTTP;
uSSDP *SSDP;

SSDP_DEVICE device;

EthernetClient *response;
unsigned long time = 0;

void index(EthernetClient *response);
void schema(EthernetClient *response);

void setup(){
	#if DEBUG > 0
	Serial.begin(9600);
	#endif

	Ethernet.begin(macaddr, ipaddr, gateway);

	#if DEBUG > 0
	Serial.print(F("Starting uHTTP at "));
	Serial.println(ipaddr);
	#endif
	HTTP = new uHTTP(80);

	#if DEBUG > 0
	Serial.println(F("Starting uSSDP..."));
	#endif

	strcpy_P(device.model, PSTR("DOMO"));
	strcpy_P(device.name, PSTR("DOMO"));
	strcpy_P(device.uuid, PSTR("abcdefgh-7dec-11d0-a765-7499692d3040"));
	strcpy_P(device.addr, PSTR("192.168.0.254"));
	device.version = 1;
	SSDP = new uSSDP(device);
}

void loop(){

	// Process HTTP request
	if((response = HTTP->process())){
		if(strcmp_P(HTTP->uri(), PSTR("/ssdp/schema.xml")) == 0){
			schema(response);
		}else{
			index(response);
		}
		response->stop();
	}

	// Process SSDP request
	if(SSDP->process()){
		if(SSDP->method() == METHOD_MSEARCH){
			SSDP_HEADER head = SSDP->head();	
			if(strncmp_P(head.st, PSTR("Basic:1"), 28) == 0 || strcmp_P(head.st, PSTR("ssdp:all")) == 0){
				SSDP->send(SSDP_RESPONSE);	
			}
		}
	}

	if(time == 0 || (millis() - time) > (1200 * 1000L)){
		time = millis();
		SSDP->send(SSDP_NOTIFY);
	}
}

void schema(EthernetClient *response){
  	response->println(F("HTTP/1.1 200 OK"));
	response->println(F("Content-Type: text/xml"));
	response->println();
	response->println(F("<?xml version=\"1.0\"?>"));
	response->println(F("\t<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"));
	response->println(F("\t\t<specVersion>"));
	response->println(F("\t\t\t<major>1</major>"));
	response->println(F("\t\t\t<minor>0</minor>"));
	response->println(F("\t\t</specVersion>"));
	response->println(F("\t\t<URLBase>/</URLBase>"));
	response->println(F("\t\t<device>"));
	response->println(F("\t\t\t<deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType>"));
	response->println("\t\t\t<friendlyName>" + String(device.name) + "</friendlyName>");	
	response->println(F("\t\t\t<manufacturer>Nomad NT</manufacturer>"));
	response->println(F("\t\t\t<manufacturerURL>http://nomadnt.com</manufacturerURL>"));
	response->println("\t\t\t<modelName>" + String(device.model) + "</modelName>");
	response->println("\t\t\t<modelNumber>" + String(device.version) + "</modelNumber>");
	response->println(F("\t\t\t<serialNumber>1</serialNumber>"));
	response->println("\t\t\t<UDN>uuid:" + String(device.uuid) + "</UDN>");
	response->println(F("\t\t</device>"));
	response->println(F("\t</root>"));
}

void index(EthernetClient *response){
	response->println(F("HTTP/1.1 200 OK"));
	response->println(F("Content-Type: text/html"));
	response->println();
	response->println(F("Hello World!"));
}