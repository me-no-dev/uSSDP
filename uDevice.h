/*
  uDevice.h - Library that implement Device for SSDP protocol.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#ifndef uDEVICE_H
#define uDEVICE_H

#include <Arduino.h>
#include <avr/pgmspace.h>

#define UUIDBASE_SIZE           24
#define PRESENTATIONURL_SIZE    32
#define FRIENDLYNAME_SIZE       32
#define MODELNAME_SIZE          32
#define SERIALNUMBER_SIZE       32
#define MANUFACTURER_SIZE       32
#define MANUFACTURERURL_SIZE    32

typedef enum device_t  { BASIC, MANAGEABLE, SOLARPROTECTIONBLIND, DIGITALSECURITYCAMERA, HVAC, LIGHTINGCONTROL };

typedef struct version_t{
  uint8_t major;
  uint8_t minor;
};

class uDevice{

  public:
    uDevice(const PROGMEM char *base, byte *mac);
    uDevice(const PROGMEM char *base, byte *mac, device_t deviceType);
    ~uDevice();

    char *uuid();

    device_t deviceType();
    void deviceType(device_t deviceType);

    char *presentationURL();
    void presentationURL(char *presentationURL);
    void presentationURL(const PROGMEM char *presentationURL);
    void presentationURL(const __FlashStringHelper *presentationURL);

    char *friendlyName();
    void friendlyName(char *friendlyName);
    void friendlyName(const PROGMEM char *friendlyName);
    void friendlyName(const __FlashStringHelper *friendlyName);

    char *modelName();
    void modelName(char *modelName);
    void modelName(const PROGMEM char *modelName);
    void modelName(const __FlashStringHelper *modelName);

    version_t modelNumber();
    void modelNumber(uint8_t major, uint8_t minor);

    char *serialNumber();
    void serialNumber(char *serialNumber);
    void serialNumber(const PROGMEM char *serialNumber);
    void serialNumber(const __FlashStringHelper *serialNumber);

    char *manufacturer();
    void manufacturer(char *manufacturer);
    void manufacturer(const PROGMEM char *manufacturer);
    void manufacturer(const __FlashStringHelper *manufacturer);

    char *manufacturerURL();
    void manufacturerURL(char *manufacturerURL);
    void manufacturerURL(const PROGMEM char *manufacturerURL);
    void manufacturerURL(const __FlashStringHelper *manufacturerURL);

  private:
    byte *_mac;
    char *_base;

    device_t _deviceType;
    version_t _modelNumber;

    char *_presentationURL;
    char *_friendlyName;
    char *_manufacturer;
    char *_manufacturerURL;
    char *_modelName;
    char *_serialNumber;
};

#endif