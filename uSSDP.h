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
#include <uDevice.h>

#define DEBUG           0

#define SSDP_INTERVAL   1200
#define SSDP_PORT       1900

// Sizes
#define METHOD_SIZE     10
#define URI_SIZE        2
#define BUFFER_SIZE     48

class uSSDP{

  typedef enum {NONE, SEARCH, NOTIFY} method_t;

  public:
    uSSDP(uDevice *device);
    ~uSSDP();

    uint8_t process();
    void send(method_t method);
    void schema(EthernetClient *client);

  private:
    EthernetUDP _server;

    uDevice *_device;

    bool _pending;
    unsigned short _delay;
    unsigned long _process_time;
    unsigned long _notify_time;
};

#endif