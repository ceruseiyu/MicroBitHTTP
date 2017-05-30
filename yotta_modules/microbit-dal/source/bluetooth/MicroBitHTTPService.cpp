#include "MicroBitConfig.h"
#include "ble/UUID.h"

#include "MicroBitHTTPService.h"
#include "MicroBitEvent.h"
#include "MicroBitFiber.h"


MicroBitHTTPService::MicroBitHTTPService(BLEDevice &_ble) : ble(_ble) {
  // Allow reading and writing to the characteristics
  uint8_t characteristicProperties = GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY;

  // Create all characteristics and place them in an array
  GattCharacteristic urlCharacteristic(MicroBitHTTPServiceUrlUUID, (uint8_t*)urlCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);
  GattCharacteristic requestCharacteristic(MicroBitHTTPServiceRequestUUID, (uint8_t*)requestCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);
  GattCharacteristic responseCharacteristic(MicroBitHTTPServiceResponseUUID, (uint8_t*)responseCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);
  GattCharacteristic postDataCharacteristic(MicroBitHTTPServicePostDataUUID, (uint8_t*)postDataCharacteristicBuffer, MAX_BYTES, MAX_BYTES, characteristicProperties);

  GattCharacteristic *characteristics[] = {&urlCharacteristic, &requestCharacteristic, &responseCharacteristic, &postDataCharacteristic};

  //Initialise buffers to 0
  memset(urlCharacteristicBuffer, 0, MAX_BYTES);
  memset(requestCharacteristicBuffer, 0, MAX_BYTES);
  memset(responseCharacteristicBuffer, 0, MAX_BYTES);
  memset(postDataCharacteristicBuffer, 0, MAX_BYTES);

  // Add security to characteristics
  urlCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  requestCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  responseCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
  postDataCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

  // Create service object and add it to the BLEDevice to start advertising
  GattService service(MicroBitHTTPServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic*));
  ble.addService(service);

  // Get handles to reference the characteristics later
  urlCharacteristicHandle = urlCharacteristic.getValueHandle();
  requestCharacteristicHandle = requestCharacteristic.getValueHandle();
  responseCharacteristicHandle = responseCharacteristic.getValueHandle();
  postDataCharacteristicHandle = postDataCharacteristic.getValueHandle();

  // Bind onDataWritten function to the onDataWritten event called when characteristics are written to
  ble.onDataWritten(this, &MicroBitHTTPService::onDataWritten);
}

// Event function called when characteristics are written to
void MicroBitHTTPService::onDataWritten(const GattWriteCallbackParams *params) {
  if(params->handle == responseCharacteristicHandle) { // When receiving response from Raspberry Pi
    responseData = (uint8_t*)params->data; // Store the response and length
    responseLen = params->len;
    MicroBitEvent(MICROBIT_ID_BLE_HTTP, MICROBIT_BLE_HTTP_RECEIVED); // Unlock the fiber for requestHTTP
  }
}

// Write string to the URL characteristic
HTTP_ERROR MicroBitHTTPService::setURL(ManagedString url) {
  if(url.length() < MAX_BYTES) { // Check string is not larger than MTU
    uint8_t* urlBytes = (uint8_t*)url.toCharArray();
    if (ble.getGapState().connected) {
      ble.gattServer().notify(urlCharacteristicHandle, urlBytes, url.length());
    } else {
      ble.gattServer().write(urlCharacteristicHandle, urlBytes, url.length());
    }
    return NO_ERROR;
  }
  return URL_TOO_LARGE;
}

// Make a HTTP request
uint8_t* MicroBitHTTPService::requestHTTP(HTTP_TYPE type, ManagedString field) {
  if(field.length() > MAX_BYTES - 1) { // Check field will not exceed MTU
    return NULL;
  }
  ManagedString message;
  switch(type) { // Place first character for the Raspberry Pi to switch on later
    case HTTP_GET:
      message = "G" + field;
      break;
    case HTTP_POST:
      message = "P" + field;
      break;
    case HTTP_PUT:
      message = "p" + field;
      break;
    case HTTP_DELETE:
      message = "D" + field;
      break;
  }
  writeRequest(message); // Write the request
  //Now prepare to receive the message and return it by locking the fiber until the event is called.
  fiber_wait_for_event(MICROBIT_ID_BLE_HTTP, MICROBIT_BLE_HTTP_RECEIVED);

  /*If you don't +1 to responseLen when allocating and clearing the allocated
    memory, you end up with a memory error with random characters being added
  */
  uint8_t* returnData = (uint8_t*)malloc(sizeof(uint8_t) * responseLen + 1); // Allocate copy of data and return it
  memset(returnData, 0, responseLen + 1);
  memcpy(returnData, responseData, responseLen);
  return returnData;
}

// Make a HTTP request using a server side macro
uint8_t* MicroBitHTTPService::requestMacroHTTP(uint8_t macroID, ManagedString macroParams) {
  if(macroParams.length() > MAX_BYTES - 2) { // Check MTU will not be exceeded
    return NULL;
  }

  // Build the string containing the macro verb, ID and parameters
  char* messageArray = (char*)malloc(macroParams.length() + 1);
  messageArray[0] = 'M';
  messageArray[1] = (char)macroID;
  messageArray[2] = '\0';
  strcat(messageArray, macroParams.toCharArray());

  writeRequest(messageArray);
  // Lock fiber until the event is called.
  fiber_wait_for_event(MICROBIT_ID_BLE_HTTP, MICROBIT_BLE_HTTP_RECEIVED);

  /*If you don't +1 to responseLen when allocating and clearing the allocated
    memory, you end up with a memory error with random characters being added
  */
  uint8_t* returnData = (uint8_t*)malloc(sizeof(uint8_t) * responseLen + 1); // Allocate copy of data and return it
  memset(returnData, 0, responseLen + 1);
  memcpy(returnData, responseData, responseLen);
  return returnData;
}

// Write message to the Request characteristic
void MicroBitHTTPService::writeRequest(ManagedString message) {
  uint8_t* messageBytes = (uint8_t*)message.toCharArray();
  if (ble.getGapState().connected) {
    ble.gattServer().notify(requestCharacteristicHandle, messageBytes, message.length());
  } else {
    ble.gattServer().write(requestCharacteristicHandle, messageBytes, message.length());
  }
}

// Write data to the PostData characteristic
HTTP_ERROR MicroBitHTTPService::writePostData(ManagedString data) {
  if(data.length() < MAX_BYTES) { // Check MTU is not exceeded
    uint8_t* dataBytes = (uint8_t*)data.toCharArray();
    if (ble.getGapState().connected) {
      ble.gattServer().notify(postDataCharacteristicHandle, dataBytes, data.length());
    } else {
      ble.gattServer().write(postDataCharacteristicHandle, dataBytes, data.length());
    }
    return NO_ERROR;
  }
  return DATA_TOO_LARGE;
}

// Service UUID
const uint8_t MicroBitHTTPServiceUUID[] = {
  0x13, 0x51, 0x63, 0x4a, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};

//Characteristic UUIDs
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
