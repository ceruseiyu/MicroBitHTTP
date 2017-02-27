#include "MicroBitConfig.h"
#include "ble/UUID.h"

#include "MicroBitHTTPService.h"

MicroBitHTTPService::MicroBitHTTPService(BLEDevice &_ble) : ble(_ble) {

  uint8_t characteristicProperties = GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE;

  /*GattCharacteristic urlCharacteristic(MicroBitHTTPServiceUrlUUID, (uint8_t*)urlCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);
  GattCharacteristic requestCharacteristic(MicroBitHTTPServiceRequestUUID, (uint8_t*)requestCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);
  GattCharacteristic responseCharacteristic(MicroBitHTTPServiceResponseUUID, (uint8_t*)responseCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);

  GattCharacteristic *characteristics[] = {&urlCharacteristic, &requestCharacteristic, &responseCharacteristic};

  memset(urlCharacteristicBuffer, 0, MAX_BYTES);
  memset(requestCharacteristicBuffer, 0, MAX_BYTES);
  memset(responseCharacteristicBuffer, 0, MAX_BYTES);*/

  GattCharacteristic urlCharacteristic(MicroBitHTTPServiceUrlUUID, urlCharacteristicBuffer, 1, MAX_BYTES, characteristicProperties);
  GattCharacteristic requestCharacteristic(MicroBitHTTPServiceRequestUUID, requestCharacteristicBuffer, 1, MAX_BYTES, characteristicProperties);
  GattCharacteristic responseCharacteristic(MicroBitHTTPServiceResponseUUID, responseCharacteristicBuffer, 1, MAX_BYTES, characteristicProperties);

  GattCharacteristic *characteristics[] = {&urlCharacteristic, &requestCharacteristic, &responseCharacteristic};

  memset(urlCharacteristicBuffer, 0, 1);
  memset(requestCharacteristicBuffer, 0, 1);
  memset(responseCharacteristicBuffer, 0, 1);


  urlCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  requestCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  responseCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

  GattService service(MicroBitHTTPServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic*));
  ble.addService(service);

  urlCharacteristicHandle = urlCharacteristic.getValueHandle();
  requestCharacteristicHandle = requestCharacteristic.getValueHandle();
  responseCharacteristicHandle = responseCharacteristic.getValueHandle();

  ble.onDataWritten(this, &MicroBitHTTPService::onDataWritten);

}

void MicroBitHTTPService::onDataWritten(const GattWriteCallbackParams *params) {

}

void MicroBitHTTPService::setURL(ManagedString url) {
  if(url.length() < MAX_BYTES) {
    uint8_t* urlBytes = (uint8_t*)url.toCharArray();
    ble.gattServer().write(urlCharacteristicHandle, urlBytes, url.length());
  }
}

uint8_t* MicroBitHTTPService::requestHTTP(HTTP_TYPE type, ManagedString field) {
  if(field.length() > MAX_BYTES - 1) {
    return NULL;
  }
  ManagedString message;
  switch(type) {
    case HTTP_GET:  
      message = "G" + field;
      writeRequest(message);
      break;
    case HTTP_POST: 
      message = "P" + field;
      writeRequest(message);
      break;
    case HTTP_PUT: 
      message = "p" + field;
      writeRequest(message);
      break;
    case HTTP_DELETE: 
      message = "D" + field;
      writeRequest(message);
      break;
  }

  //Now prepare to receive the message and return it.

  return NULL;
}

void MicroBitHTTPService::writeRequest(ManagedString message) {
  uint8_t* messageBytes = (uint8_t*)message.toCharArray();
  ble.gattServer().write(requestCharacteristicHandle, messageBytes, message.length());
}

const uint8_t MicroBitHTTPServiceUUID[] = {
  0x13, 0x51, 0x63, 0x4a, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};

const uint8_t MicroBitHTTPServiceUrlUUID[] = {
  0x13, 0x51, 0x3f, 0x02, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};

const uint8_t MicroBitHTTPServiceRequestUUID[] = {
  0x13, 0x51, 0x14, 0x9e, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};

const uint8_t MicroBitHTTPServiceResponseUUID[] = {
  0x13, 0x51, 0x57, 0x80, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};