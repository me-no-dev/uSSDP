/**
 * SSDP Device example
 * Process SSDP request and NOTIFY services and presence
 * 
 * This example code is in the public domain.
 **/

#include <SPI.h>
#include <Ethernet.h>
#include <uHTTP.h>
#include <uSSDP.h>

#define DEBUG 1

PROGMEM const char UUID_BASE[] = "abcdefgh-7dec-11d0-a765";
byte macaddr[6] = { 0x74, 0x99, 0x69, 0x2D, 0x30, 0x40 };
byte ip4addr[4] = { 192, 168, 0, 254 };

uDevice *device;

uHTTP *HTTP;
uSSDP *SSDP;

EthernetClient *response;
unsigned long time = 0;

void setup(){
	#if DEBUG > 0
	Serial.begin(9600);
	#endif

	device = new uDevice(UUID_BASE, macaddr);
	device->presentationURL(F("/"));
	device->friendlyName(F("DOMO"));
	device->modelName(F("DOMO"));
	device->modelNumber(1, 0);
	device->serialNumber(F("A0123456789"));
	device->manufacturer(F("Nomad NT"));
	device->manufacturerURL(F("http://nomadnt.com"));

	if(!Ethernet.begin(macaddr)){
		Ethernet.begin(macaddr, ip4addr);
	}

	#if DEBUG > 0
	Serial.print(F("Starting uHTTP at "));
	Serial.print(Ethernet.localIP());
	Serial.print(F(":"));
	Serial.println(80);
	#endif

	HTTP = new uHTTP();
	
	#if DEBUG > 0
	Serial.println(F("Starting uSSDP..."));
	#endif

	SSDP = new uSSDP(device);
}

void loop(){
	// Process HTTP request
	if((response = HTTP->process())){
		if(HTTP->uri_equals(F("/ssdp/schema.xml"))) SSDP->schema(response);
		response->stop();
	}

	// Process SSDP request
	SSDP->process();
}