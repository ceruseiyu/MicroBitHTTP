#include "MicroBitConfig.h"
#include "ble/UUID.h"

#include "MicroBitHTTPService.h"
#include "MicroBitEvent.h"
#include "MicroBitFiber.h"


MicroBitHTTPService::MicroBitHTTPService(BLEDevice &_ble) : ble(_ble) {

  uint8_t characteristicProperties = GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE;

  GattCharacteristic urlCharacteristic(MicroBitHTTPServiceUrlUUID, (uint8_t*)urlCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);
  GattCharacteristic requestCharacteristic(MicroBitHTTPServiceRequestUUID, (uint8_t*)requestCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);
  GattCharacteristic responseCharacteristic(MicroBitHTTPServiceResponseUUID, (uint8_t*)responseCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);
  GattCharacteristic postDataCharacteristic(MicroBitHTTPServicePostDataUUID, (uint8_t*)postDataCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);

  GattCharacteristic *characteristics[] = {&urlCharacteristic, &requestCharacteristic, &responseCharacteristic, &postDataCharacteristic};

  memset(urlCharacteristicBuffer, 0, MAX_BYTES);
  memset(requestCharacteristicBuffer, 0, MAX_BYTES);
  memset(responseCharacteristicBuffer, 0, MAX_BYTES);
  memset(postDataCharacteristicBuffer, 0, MAX_BYTES);

  urlCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  requestCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  responseCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  postDataCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

  GattService service(MicroBitHTTPServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic*));
  ble.addService(service);

  urlCharacteristicHandle = urlCharacteristic.getValueHandle();
  requestCharacteristicHandle = requestCharacteristic.getValueHandle();
  responseCharacteristicHandle = responseCharacteristic.getValueHandle();
  postDataCharacteristicHandle = postDataCharacteristic.getValueHandle();

  ble.onDataWritten(this, &MicroBitHTTPService::onDataWritten);

}

void MicroBitHTTPService::onDataWritten(const GattWriteCallbackParams *params) {
  if(params->handle == responseCharacteristicHandle) {
    responseData = (uint8_t*)params->data;
    responseLen = params->len;
    MicroBitEvent(MICROBIT_ID_BLE_HTTP, MICROBIT_BLE_HTTP_RECEIVED);
  }
}

HTTP_ERROR MicroBitHTTPService::setURL(ManagedString url) {
  if(url.length() < MAX_BYTES) {
    uint8_t* urlBytes = (uint8_t*)url.toCharArray();
    ble.gattServer().write(urlCharacteristicHandle, urlBytes, url.length());
    return NO_ERROR;
  }
  return URL_TOO_LARGE;
}

uint8_t* MicroBitHTTPService::requestHTTP(HTTP_TYPE type, ManagedString field) {
  if(field.length() > MAX_BYTES - 1) {
    return NULL;
  }
  ManagedString message;
  switch(type) {
    case HTTP_GET:
      message = "G" + field;
      break;
    case HTTP_POST:
      message = "P" + field;
      /*if(postData == " " || postData.length() > MAX_BYTES) {
        return NULL;
      }
      writePostData(postData);*/
      break;
    case HTTP_PUT:
      message = "p" + field;
      break;
    case HTTP_DELETE:
      message = "D" + field;
      break;
  }
  writeRequest(message);
  //Now prepare to receive the message and return it.
  fiber_wait_for_event(MICROBIT_ID_BLE_HTTP, MICROBIT_BLE_HTTP_RECEIVED);

  /*If you don't +1 to responseLen when allocating and clearing the allocated
    memory, you end up with a memory error with random characters being added
  */
  uint8_t* returnData = (uint8_t*)malloc(sizeof(uint8_t) * responseLen + 1);
  memset(returnData, 0, responseLen + 1);
  memcpy(returnData, responseData, responseLen);
  return returnData;
}

uint8_t* MicroBitHTTPService::requestMacroHTTP(uint8_t macroID, ManagedString macroParams) {
  if(macroParams.length() > MAX_BYTES - 2) {
    return NULL;
  }
  char* messageArray = (char*)malloc(macroParams.length() + 1);
  messageArray[0] = 'M';
  messageArray[1] = (char)macroID;
  messageArray[2] = '\0';
  strcat(messageArray, macroParams.toCharArray());
  writeRequest(messageArray);
  fiber_wait_for_event(MICROBIT_ID_BLE_HTTP, MICROBIT_BLE_HTTP_RECEIVED);

  uint8_t* returnData = (uint8_t*)malloc(sizeof(uint8_t) * responseLen + 1);
  memset(returnData, 0, responseLen + 1);
  memcpy(returnData, responseData, responseLen);
  return returnData;
}

void MicroBitHTTPService::writeRequest(ManagedString message) {
  uint8_t* messageBytes = (uint8_t*)message.toCharArray();
  ble.gattServer().write(requestCharacteristicHandle, messageBytes, message.length());
}

HTTP_ERROR MicroBitHTTPService::writePostData(ManagedString data) {
  if(data.length() < MAX_BYTES) {
    uint8_t* dataBytes = (uint8_t*)data.toCharArray();
    ble.gattServer().write(postDataCharacteristicHandle, dataBytes, data.length());
    return NO_ERROR;
  }
  return DATA_TOO_LARGE;
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

const uint8_t MicroBitHTTPServicePostDataUUID[] = {
  0x13, 0x51, 0x4c, 0x6f, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};
