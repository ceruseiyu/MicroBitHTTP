#ifndef MICROBIT_HTTP_SERVICE_H
#define MICROBIT_HTTP_SERVICE_H


#include "MicroBitConfig.h"
#include "ble/UUID.h"
#include "ble/BLE.h"

extern const uint8_t MicroBitHTTPServiceUUID[];
extern const uint8_t MicroBitHTTPServiceUrlUUID[];

class MicroBitHTTPService {
  public:
    MicroBitHTTPService(BLEDevice &_ble);
    void onDataWritten(const GattWriteCallbackParams * params);
    void updateURL(char* url);

  private:
    BLEDevice &ble;

    char* urlCharacteristicBuffer;

    GattAttribute::Handle_t urlCharacteristicHandle;
};

#endif
