/*
  uSSDP.cpp - Library that implement SSDP protocol.
  Created by Filippo Sallemi, July 23, 2014.
  Released into the public domain.
*/

#include "uDevice.h"

uDevice::uDevice(const PROGMEM char *base, byte *mac){
  _base = new char[UUIDBASE_SIZE];
  _presentationURL = new char[PRESENTATIONURL_SIZE];
  _friendlyName = new char[FRIENDLYNAME_SIZE];
  _modelName = new char[MODELNAME_SIZE];
  _serialNumber = new char[SERIALNUMBER_SIZE];
  _manufacturer = new char[MANUFACTURER_SIZE];
  _manufacturerURL = new char[MANUFACTURERURL_SIZE];

  _mac = mac;
  _deviceType = BASIC;
  _modelNumber = {0, 0};

  strcpy_P(_base, base);
}

uDevice::uDevice(const PROGMEM char *base, byte *mac, device_t deviceType){
  _base = new char[UUIDBASE_SIZE];
  _presentationURL = new char[PRESENTATIONURL_SIZE];
  _friendlyName = new char[FRIENDLYNAME_SIZE];
  _modelName = new char[MODELNAME_SIZE];
  _serialNumber = new char[SERIALNUMBER_SIZE];
  _manufacturer = new char[MANUFACTURER_SIZE];
  _manufacturerURL = new char[MANUFACTURERURL_SIZE];

  _mac = mac;
  _deviceType = deviceType;

  strcpy_P(_base, base);
}

uDevice::~uDevice(){
	delete [] _base;
  delete [] _mac;
  delete [] _presentationURL;
  delete [] _friendlyName;
  delete [] _modelName;
  delete [] _serialNumber;
  delete [] _manufacturer;
  delete [] _manufacturerURL;
}

char *uDevice::uuid(){
  char *_uuid = new char[37];
  sprintf(_uuid, "%s-%02X%02X%02X%02X%02X%02X", _base, _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
	return _uuid;
}

device_t uDevice::deviceType(){
  return _deviceType;
}

void uDevice::deviceType(device_t deviceType){
  _deviceType = deviceType;
}

char *uDevice::presentationURL(){
  return _presentationURL;
}

void uDevice::presentationURL(char *presentationURL){
  _presentationURL = presentationURL;
}

void uDevice::presentationURL(const PROGMEM char *presentationURL){
  strcpy_P(_presentationURL, presentationURL);
}

void uDevice::presentationURL(const __FlashStringHelper *presentationURL){
  this->presentationURL((const PROGMEM char *) presentationURL);
}

char *uDevice::friendlyName(){
  return _friendlyName;
}

void uDevice::friendlyName(char *friendlyName){
  _friendlyName = friendlyName;
}

void uDevice::friendlyName(const PROGMEM char *friendlyName){
  strcpy_P(_friendlyName, friendlyName);
}

void uDevice::friendlyName(const __FlashStringHelper *friendlyName){
  this->friendlyName((const PROGMEM char *) friendlyName);
}

char *uDevice::modelName(){
  return _modelName;
}

void uDevice::modelName(char *modelName){
  _modelName = modelName;
}

void uDevice::modelName(const PROGMEM char *modelName){
  strcpy_P(_modelName, modelName);
}

void uDevice::modelName(const __FlashStringHelper *modelName){
  this->modelName((const PROGMEM char *) modelName);
}

version_t uDevice::modelNumber(){
  return _modelNumber;
}

void uDevice::modelNumber(uint8_t major, uint8_t minor){
  _modelNumber.major = major;
  _modelNumber.minor = minor;
}

char *uDevice::serialNumber(){
  return _serialNumber;
}

void uDevice::serialNumber(char *serialNumber){
  _serialNumber = serialNumber;
}

void uDevice::serialNumber(const PROGMEM char *serialNumber){
  strcpy_P(_serialNumber, serialNumber);
}

void uDevice::serialNumber(const __FlashStringHelper *serialNumber){
  this->serialNumber((const PROGMEM char *) serialNumber);
}

char *uDevice::manufacturer(){
  return _manufacturer;
}

void uDevice::manufacturer(char *manufacturer){
  _manufacturer = manufacturer;
}

void uDevice::manufacturer(const PROGMEM char *manufacturer){
  strcpy_P(_manufacturer, manufacturer);
}

void uDevice::manufacturer(const __FlashStringHelper *manufacturer){
  this->manufacturer((const PROGMEM char *) manufacturer);
}

char *uDevice::manufacturerURL(){
  return _manufacturerURL;
}

void uDevice::manufacturerURL(char *manufacturerURL){
  _manufacturerURL = manufacturerURL;
}

void uDevice::manufacturerURL(const PROGMEM char *manufacturerURL){
  strcpy_P(_manufacturerURL, manufacturerURL);
}

void uDevice::manufacturerURL(const __FlashStringHelper *manufacturerURL){
  this->manufacturerURL((const PROGMEM char *) manufacturerURL);
}