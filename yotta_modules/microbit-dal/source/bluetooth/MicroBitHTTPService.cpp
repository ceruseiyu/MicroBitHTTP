#include "MicroBitConfig.h"
#include "ble/UUID.h"

#include "MicroBitHTTPService.h"

MicroBitHTTPService::MicroBitHTTPService(BLEDevice &_ble) : ble(_ble) {
  uint8_t characteristicProperties = GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE;
  GattCharacteristic urlCharacteristic(MicroBitHTTPServiceUrlUUID, (uint8_t*)urlCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);
  GattCharacteristic requestCharacteristic(MicroBitHTTPServiceRequestUUID, (uint8_t*)requestCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);
  GattCharacteristic *characteristics[] = {&urlCharacteristic, &requestCharacteristic};

  //urlCharacteristicBuffer = (uint8_t*)malloc(20);
  //requestCharacteristicBuffer = (uint8_t*)malloc(20);
  memset(urlCharacteristicBuffer, 0, MAX_BYTES);
  memset(requestCharacteristicBuffer, 0, MAX_BYTES);


  urlCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  requestCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

  GattService service(MicroBitHTTPServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic*));
  ble.addService(service);

  urlCharacteristicHandle = urlCharacteristic.getValueHandle();
  requestCharacteristicHandle = requestCharacteristic.getValueHandle();

  ble.onDataWritten(this, &MicroBitHTTPService::onDataWritten);

}

void MicroBitHTTPService::onDataWritten(const GattWriteCallbackParams *params) {

}

void MicroBitHTTPService::updateURL(ManagedString url) {
  if(url.length() < MAX_BYTES) {
    uint8_t* urlBytes = (uint8_t*)url.toCharArray();
    //memcpy(urlCharacteristicBuffer, urlBytes, url.length());
    ble.gattServer().write(urlCharacteristicHandle, urlBytes, url.length());
  }
}

uint8_t* MicroBitHTTPService::requestHTTP(HTTP_TYPE type, char* field) {
  char* requestMessage = (char*)malloc(MAX_BYTES); 
  switch(type) {
    case HTTP_GET:  
      requestMessage[0] = 'G';
      requestMessage[1] = '\0';
      if(strlen(field) < MAX_BYTES - 1) {
        //strcat(requestMessage, field);
        //strcpy(requestMessage, requestCharacteristicBuffer);
        ble.gattServer().write(requestCharacteristicHandle, (uint8_t*)&requestCharacteristicBuffer, sizeof(requestCharacteristicBuffer));

        //Now prepare to receive the data and then return it 
        
      } else {
        return NULL;
      }
      break;
    case HTTP_POST: 
      requestMessage[0] = 'P';
      break;
    case HTTP_PUT: 
      requestMessage[0] = 'p';
      break;
    case HTTP_DELETE: 
      requestMessage[0] = 'D';
      break;
  }

  return NULL;
}
const uint8_t MicroBitHTTPServiceUUID[] = {
  0x13, 0x51, 0x63, 0x4a, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};

const uint8_t MicroBitHTTPServiceUrlUUID[] = {
  0x13, 0x51, 0x3f, 0x02, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};

const uint8_t MicroBitHTTPServiceRequestUUID [] = {
  0x13, 0x51, 0x14, 0x9e, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};