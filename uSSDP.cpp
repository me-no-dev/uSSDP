/*
  uSSDP.cpp - Library that implement SSDP protocol.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#include "uSSDP.h"

uSSDP::uSSDP(uDevice *device){
  _device = device;
  _pending = false;
  _server.begin(SSDP_PORT);
}

uSSDP::~uSSDP(){
  delete _device;
}

uint8_t uSSDP::process(){
  if(!_pending && _server.parsePacket() > 0){
    method_t method = NONE;

    typedef enum {METHOD, URI, PROTO, KEY, VALUE, ABORT} states;
    states state = METHOD;

    typedef enum {START, MAN, ST, MX} headers;
    headers header = START;

    uint8_t cursor = 0;
    uint8_t cr = 0;

    char buffer[BUFFER_SIZE] = {0};
    
    while(_server.available() > 0){
      char c = _server.read();

      (c == '\r' || c == '\n') ? cr++ : cr = 0;

      switch(state){
        case METHOD:
          if(c == ' '){
            if(strcmp_P(buffer, PSTR("M-SEARCH")) == 0) method = SEARCH;
            else if(strcmp_P(buffer, PSTR("NOTIFY")) == 0) method = NOTIFY;
            
            if(method == NONE) state = ABORT;
            else state = URI; 
            cursor = 0;

          }else if(cursor < METHOD_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
          break;
        case URI:
          if(c == ' '){
            if(strcmp_P(buffer, PSTR("*"))) state = ABORT;
            else state = PROTO; 
            cursor = 0; 
          }else if(cursor < URI_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
          break;
        case PROTO:
          if(cr == 2){ state = KEY; cursor = 0; }
          break;
        case KEY:
          if(cr == 4){ _pending = true; _process_time = millis(); }
          else if(c == ' '){ cursor = 0; state = VALUE; }
          else if(c != '\r' && c != '\n' && c != ':' && cursor < BUFFER_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
          break;
        case VALUE:
          if(cr == 2){
            switch(header){
              case MAN:
                //Serial.print("MAN: ");
                //Serial.println(buffer);
                //strncpy(_head.man, buffer, HEAD_VAL_SIZE);
                break;
              case ST:
                if(strcmp_P(buffer, PSTR("ssdp:all"))){
                  state = ABORT;
                  #if DEBUG > 0
                  Serial.print("REJECT: ");
                  Serial.println(buffer);
                  #endif
                }
                break;
              case MX:
                _delay = random(0, atoi(buffer)) * 1000L;
                break;
            }

            if(state != ABORT){ state = KEY; header = START; cursor = 0; }
          }else if(c != '\r' && c != '\n'){
            if(header == START){
              if(strncmp_P(buffer, PSTR("MA"), 2) == 0) header = MAN;
              else if(strcmp_P(buffer, PSTR("ST")) == 0) header = ST;
              else if(strcmp_P(buffer, PSTR("MX")) == 0) header = MX;
            }
            
            if(cursor < BUFFER_SIZE - 1){ buffer[cursor++] = c; buffer[cursor] = '\0'; }
          }
          break;
        case ABORT:
          _pending = false; _delay = 0;
          break;
      }
    }
    
    _server.flush();
  }

  if(_pending && (millis() - _process_time) > _delay){
    _pending = false; _delay = 0;
    send(NONE);
  }else if(_notify_time == 0 || (millis() - _notify_time) > (SSDP_INTERVAL * 1000L)){
    _notify_time = millis();
    send(NOTIFY);
  }
}

void uSSDP::send(method_t method){
  version_t modelNumber = _device->modelNumber();
  byte ssdp[4] = {239, 255, 255, 250};

	if(method == NONE){
    #if DEBUG > 0
    Serial.print(F("Sending Response to "));
    Serial.print(_server.remoteIP());
    Serial.print(F(":"));
    Serial.println(_server.remotePort());
    #endif

    _server.beginPacket(_server.remoteIP(), _server.remotePort());
		_server.println(F("HTTP/1.1 200 OK"));
		_server.println(F("EXT:"));
		_server.println(F("ST: upnp:rootdevice"));
	}else if(method == NOTIFY){
    #if DEBUG > 0
    Serial.println(F("Sending Notify to 239.255.255.250:1900"));
    #endif

    _server.beginPacket(ssdp, SSDP_PORT);
		_server.println(F("NOTIFY * HTTP/1.1"));
		_server.println(F("HOST: 239.255.255.250:1900"));
		_server.println(F("NT: upnp:rootdevice"));
		_server.println(F("NTS: ssdp:alive"));
	}

	_server.print(F("CACHE-CONTROL: max-age="));
	_server.println(SSDP_INTERVAL);	

  _server.print(F("SERVER: Arduino/1.0 UPNP/1.1 "));
  _server.print(_device->modelName());
  if(modelNumber.major > 0 || modelNumber.minor > 0){
    _server.print(F("/"));
    _server.print(modelNumber.major);
    _server.print(F("."));
    _server.print(modelNumber.minor);
  }
  _server.println();
  
	_server.print(F("USN: uuid:"));
	_server.println(_device->uuid());

	_server.print(F("LOCATION: http://"));
	_server.print(Ethernet.localIP());
	_server.println(F("/ssdp/schema.xml"));
	_server.println();

	_server.endPacket();
}

void uSSDP::schema(EthernetClient *client){
  client->println(F("HTTP/1.1 200 OK"));
  client->println(F("Content-Type: text/xml"));
  client->println();

  client->println(F("<?xml version=\"1.0\"?>"));
  client->println(F("<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"));
  client->println(F("\t<specVersion>"));
  client->println(F("\t\t<major>1</major>"));
  client->println(F("\t\t<minor>0</minor>"));
  client->println(F("\t</specVersion>"));
    
  client->println(F("\t<device>"));
  client->println(F("\t\t<deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType>"));

  if(strlen(_device->presentationURL())){
    client->print(F("\t<presentationURL>"));
    client->print(_device->presentationURL());
    client->print(F("</presentationURL>\r\n"));
  }

  client->print(F("\t\t<friendlyName>"));
  client->print(_device->friendlyName());
  client->print(F("</friendlyName>\r\n"));

  client->print(F("\t\t<modelName>"));
  client->print(_device->modelName());
  client->print(F("</modelName>\r\n"));

  version_t modelNumber = _device->modelNumber();

  if(modelNumber.major > 0 || modelNumber.minor > 0){
    client->print(F("\t\t<modelNumber>"));
    client->print(modelNumber.major);
    client->print(F("."));
    client->print(modelNumber.minor);
    client->print(F("</modelNumber>\r\n"));
  }

  if(strlen(_device->serialNumber())){
    client->print(F("\t\t<serialNumber>"));
    client->print(_device->serialNumber());
    client->print(F("</serialNumber>\r\n"));
  }

  client->print(F("\t\t<manufacturer>"));
  client->print(_device->manufacturer());
  client->print(F("</manufacturer>\r\n"));

  if(strlen(_device->manufacturerURL())){
    client->print(F("\t\t<manufacturerURL>"));
    client->print(_device->manufacturerURL());
    client->print(F("</manufacturerURL>\r\n"));
  }

  client->print(F("\t\t<UDN>uuid:"));
  client->print(_device->uuid());
  client->print(F("</UDN>\r\n"));

  client->println(F("\t</device>"));
  client->println(F("</root>"));
}