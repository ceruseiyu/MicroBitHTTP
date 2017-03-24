#ifndef MICROBIT_HTTP_SERVICE_H
#define MICROBIT_HTTP_SERVICE_H


#include "MicroBitConfig.h"
#include "ble/UUID.h"
#include "ble/BLE.h"
#include "ManagedString.h"

// MTU for BLE on Micro:bit
#define MAX_BYTES 20

//Define our component ID and event code for that component
#define MICROBIT_ID_BLE_HTTP 1200
#define MICROBIT_BLE_HTTP_RECEIVED 1


// HTTP verbs to give to the requestHTTP function
enum HTTP_TYPE {
  HTTP_GET,
  HTTP_POST,
  HTTP_PUT,
  HTTP_DELETE
};

// Error values returned from setURL and writePostData
enum HTTP_ERROR {
  NO_ERROR,
  URL_TOO_LARGE,
  DATA_TOO_LARGE
};

// Service UUID
extern const uint8_t MicroBitHTTPServiceUUID[];

// Characteristic UUIDs
extern const uint8_t MicroBitHTTPServiceUrlUUID[];
extern const uint8_t MicroBitHTTPServiceRequestUUID[];
extern const uint8_t MicroBitHTTPServiceResponseUUID[];
extern const uint8_t MicroBitHTTPServicePostDataUUID[];

class MicroBitHTTPService {
  public:
    MicroBitHTTPService(BLEDevice &_ble);
    void onDataWritten(const GattWriteCallbackParams * params);

    // Set a URL to make HTTP requests to
    HTTP_ERROR setURL(ManagedString url);

    // Set the POST data to submit with POST or PUT requests
    HTTP_ERROR writePostData(ManagedString data);

    // Make a HTTP Request
    uint8_t* requestHTTP(HTTP_TYPE type, ManagedString field);

    // Make a HTTP Request using a macro
    uint8_t* requestMacroHTTP(uint8_t macroID, ManagedString macroParams);

  private:
    //Writes message to the Request characteristic
    void writeRequest(ManagedString message);

    //Reference to the Micro:bit BLEDevice object that holds and handles all services
    BLEDevice &ble;

    //Define buffers for characteristic data
    uint8_t urlCharacteristicBuffer[MAX_BYTES];
    uint8_t requestCharacteristicBuffer[MAX_BYTES];
    uint8_t responseCharacteristicBuffer[MAX_BYTES];
    uint8_t postDataCharacteristicBuffer[MAX_BYTES];

    // Variables to hold the response and length after the event triggers so they can be returned
    uint8_t* responseData;
    int responseLen;

    // uint_!6 identifiers for characteristics to retrieve them from the BLEDevice object
    GattAttribute::Handle_t urlCharacteristicHandle;
    GattAttribute::Handle_t requestCharacteristicHandle;
    GattAttribute::Handle_t responseCharacteristicHandle;
    GattAttribute::Handle_t postDataCharacteristicHandle;
};

#endif
