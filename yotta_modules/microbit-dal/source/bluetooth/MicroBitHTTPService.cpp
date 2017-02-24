#include "MicroBitConfig.h"
#include "ble/UUID.h"

#include "MicroBitHTTPService.h"

MicroBitHTTPService::MicroBitHTTPService(BLEDevice &_ble) : ble(_ble) {
  GattCharacteristic urlCharacteristic(MicroBitHTTPServiceUrlUUID, (uint8_t*)&urlCharacteristicBuffer, 20, 20, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);
  GattCharacteristic requestCharacteristic(MicroBitHTTPServiceRequestUUID, (uint8_t*)&urlCharacteristicBuffer, 20, 20, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);
  GattCharacteristic *characteristics[] = {&urlCharacteristic, &requestCharacteristic};

  urlCharacteristicBuffer = (char*)malloc(20);
  requestCharacteristicBuffer = (char*)malloc(20);

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

void MicroBitHTTPService::updateURL(char* url) {
  strcpy(urlCharacteristicBuffer, url); 
  ble.gattServer().write(urlCharacteristicHandle, (uint8_t*)&urlCharacteristicBuffer, sizeof(urlCharacteristicBuffer));
}

uint8_t* MicroBitHTTPService::requestHTTP(HTTP_TYPE type, char* field) {
  char[20] requestMessage;
  switch(type) {
    case HTTP_GET: 
      requestMessage[0] = 'G';
      requestMessage[1] = '\0';
      if(strlen(field) < 19) {
        strcat(requestMessage, field);
        strcpy(requestMessage, requestCharacteristicBuffer);
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
}