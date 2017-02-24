#ifndef MICROBIT_HTTP_SERVICE_H
#define MICROBIT_HTTP_SERVICE_H


#include "MicroBitConfig.h"
#include "ble/UUID.h"
#include "ble/BLE.h"

enum HTTP_TYPE {HTTP_GET, HTTP_POST, HTTP_PUT, HTTP_DELETE};


extern const uint8_t MicroBitHTTPServiceUUID[];
extern const uint8_t MicroBitHTTPServiceUrlUUID[];
extern const uint8_t MicroBitHTTPServiceRequestUUID[];

class MicroBitHTTPService {
  public:
    MicroBitHTTPService(BLEDevice &_ble);
    void onDataWritten(const GattWriteCallbackParams * params);
    void updateURL(char* url);
    uint8_t* requestHTTP(HTTP_TYPE type, char* field);

  private:
    BLEDevice &ble;

    char* urlCharacteristicBuffer;
    char* requestCharacteristicBuffer;

    GattAttribute::Handle_t urlCharacteristicHandle;
    GattAttribute::Handle_t requestCharacteristicHandle;
};

#endif
