/*
  uSSDP.cpp - Library that implement SSDP protocol.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#include "uSSDP.h"

uSSDP::uSSDP(SSDP_DEVICE &device){
	this->_uri = new char[URI_SIZE];
	this->_device = device;

	server = new EthernetUDP();
}

uSSDP::~uSSDP(){
	delete [] _uri;
	delete server;
}

uint8_t uSSDP::process(){
	server->begin(SSDP_PORT);

	if(server->parsePacket() > 0){
		memset(_uri, 0, sizeof _uri);
		memset(&_head, 0, sizeof _head);

		typedef enum {METHOD, URI, PROTO, KEY, VALUE, STOP} states;
    	states state = METHOD;

    	uint8_t cursor = 0;
    	uint8_t cr = 0;

    	char method[METHOD_SIZE] = {0};

    	bool sub = false;
    	char key[HEAD_SIZE] = {0};
    	char mx[3] = {0};

    	while(server->available() > 0){
    		char c = server->read();

        	(c == '\r' || c == '\n') ? cr++ : cr = 0;
        	if(c == 'X') continue;

        	switch(state){
        		case METHOD:
        			if(c == ' '){
              			if(strncmp_P(method, PSTR("M-"), 2) == 0) _method = METHOD_MSEARCH;
              			else if(strncmp_P(method, PSTR("NO"), 2) == 0) _method = METHOD_NOTIFY;
              			else _method = METHOD_UNKNOWN;
              			state = URI; 
              			cursor = 0;
            		}else if(cursor < METHOD_SIZE - 1){ method[cursor++] = c; method[cursor] = '\0'; }
        			break;
        		case URI:
            		if(c == ' '){ state = PROTO; cursor = 0; }
            		else if(cursor < URI_SIZE - 1){
            			_uri[cursor++] = c; _uri[cursor] = '\0'; 
            		}
            		break;
            	case PROTO:
            		if(cr == 2){ state = KEY; cursor = 0; }
            		break;
            	case KEY:
            		if(cr == 4){ state = STOP; cursor = 0; }
            		else if(c == ' '){ cursor = 0; state = VALUE; }
            		else if(c != '\r' && c != '\n' && c != ':' && cursor < HEAD_SIZE - 1){ key[cursor++] = c; key[cursor] = '\0'; }
            		break;
            	case VALUE:
            		if(cr == 2){ cursor = 0; sub = false; state = KEY;}
            		else {
              			if(c != '\r' && c != '\n'){
                			if(strcmp_P(key, PSTR("HOST")) == 0){
                  				if(cursor < HEAD_HOST_SIZE - 1){ _head.host[cursor++] = c; _head.host[cursor] = '\0'; }
                			}else if(strcmp_P(key, PSTR("MAN")) == 0){
                  				if(cursor < HEAD_MAN_SIZE - 1){ _head.man[cursor++] = c; _head.man[cursor] = '\0'; }
                			}else if(strcmp_P(key, PSTR("ST")) == 0){
                  				if(cursor < HEAD_ST_SIZE - 1){ _head.st[cursor++] = c; _head.st[cursor] = '\0'; }
                			}else if(strcmp_P(key, PSTR("USER-AGENT")) == 0){
                  				if(cursor < HEAD_AGENT_SIZE - 1){ _head.agent[cursor++] = c; _head.agent[cursor] = '\0'; }
                			}else if(strcmp_P(key, PSTR("MX")) == 0){
                  				if(cursor < 2){ mx[cursor++] = c; mx[cursor] = '\0'; }
                			}
              			}
            		}
            		break;
        		case STOP:
        			Serial.println("STOP");
            		//server->flush();
            		break;
            }
    	}
		return 1;
	}
	//server->stop();
	return 0;
}

uint8_t uSSDP::method(){
	return _method;
}

char *uSSDP::uri(){
	return _uri;
}

SSDP_HEADER uSSDP::head(){
  return _head;
}

void uSSDP::send(uint8_t type){
	uint8_t success;
	do{
		success = server->beginPacket(server->remoteIP(), server->remotePort());
  		Serial.print(F("Sending Response to "));
  		Serial.print(server->remoteIP());
  		Serial.print(F(":"));
  		Serial.println(server->remotePort());
	}while(!success);

	if(type == SSDP_RESPONSE){
		server->println("HTTP/1.1 200 OK");
		server->println("EXT:");
		server->println("ST: upnp:rootdevice");
	}else if(type == SSDP_NOTIFY){
		server->println("NOTIFY * HTTP/1.1");
		server->println("HOST: 239.255.255.250:1900");
		server->println("NT: upnp:rootdevice");
		server->println("NTS: ssdp:alive");
	}

	server->print("CACHE-CONTROL: max-age=");
	server->println(SSDP_INTERVAL);	

	server->print("SERVER: ");
	server->println(_device.name);	

	server->print("USN: uuid:");
	server->println(_device.uuid);

	server->print("LOCATION: http://");
	server->print(_device.addr);
	server->println("/ssdp/schema.xml");

	server->println();
	server->endPacket();
}