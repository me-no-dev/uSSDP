/*
  uSSDP.h - Library that implement SSDP protocol.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#ifndef uSSDP_H
#define uSSDP_H

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define SSDP_INTERVAL   1200
#define SSDP_PORT       1900

#define SSDP_RESPONSE   0
#define SSDP_NOTIFY     1

// Method types
#define METHOD_UNKNOWN  0
#define METHOD_MSEARCH  1
#define METHOD_NOTIFY   2

// Sizes
#define METHOD_SIZE     12
#define URI_SIZE        2

#define HEAD_SIZE       16
#define HEAD_HOST_SIZE  22
#define HEAD_MAN_SIZE   16
#define HEAD_ST_SIZE    48
#define HEAD_AGENT_SIZE 48

typedef struct SSDP_DEVICE{
  uint8_t version;
  char model[16];
  char name[32];
  char uuid[38];
  char addr[16];
};

typedef struct SSDP_HEADER{
  char host[HEAD_HOST_SIZE];
  char man[HEAD_MAN_SIZE];
  char st[HEAD_ST_SIZE];
  char agent[HEAD_AGENT_SIZE];
  uint8_t mx;
};

class uSSDP{

  public:
    uSSDP(SSDP_DEVICE &device);
    ~uSSDP();

    uint8_t process();
    void send(uint8_t type);

    uint8_t method();
    char *uri();

    SSDP_HEADER head();

  private:
    EthernetUDP _server;
    SSDP_HEADER _head;
    SSDP_DEVICE _device;

    uint8_t _method;
    char *_uri;
};

#endif