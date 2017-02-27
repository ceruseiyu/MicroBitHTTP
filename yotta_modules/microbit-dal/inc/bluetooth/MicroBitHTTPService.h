#ifndef MICROBIT_HTTP_SERVICE_H
#define MICROBIT_HTTP_SERVICE_H


#include "MicroBitConfig.h"
#include "ble/UUID.h"
#include "ble/BLE.h"
#include "ManagedString.h"

#define MAX_BYTES 20

enum HTTP_TYPE {
  HTTP_GET, 
  HTTP_POST, 
  HTTP_PUT, 
  HTTP_DELETE
};


extern const uint8_t MicroBitHTTPServiceUUID[];
extern const uint8_t MicroBitHTTPServiceUrlUUID[];
extern const uint8_t MicroBitHTTPServiceRequestUUID[];
extern const uint8_t MicroBitHTTPServiceResponseUUID[];

class MicroBitHTTPService {
  public:
    MicroBitHTTPService(BLEDevice &_ble);
    void onDataWritten(const GattWriteCallbackParams * params);
    void setURL(ManagedString url);
    uint8_t* requestHTTP(HTTP_TYPE type, ManagedString field);

  private:
    void writeRequest(ManagedString message);

    BLEDevice &ble;

    /*uint8_t urlCharacteristicBuffer[MAX_BYTES];
    uint8_t requestCharacteristicBuffer[MAX_BYTES];
    uint8_t responseCharacteristicBuffer[MAX_BYTES];*/

    uint8_t* urlCharacteristicBuffer;
    uint8_t* requestCharacteristicBuffer;
    uint8_t* responseCharacteristicBuffer;

    GattAttribute::Handle_t urlCharacteristicHandle;
    GattAttribute::Handle_t requestCharacteristicHandle;
    GattAttribute::Handle_t responseCharacteristicHandle;
};

#endif
