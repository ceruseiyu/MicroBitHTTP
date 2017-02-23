#include "MicroBitConfig.h"
#include "ble/UUID.h"

#include "MicroBitHTTPService.h"

#define URL_LENGTH 20

MicroBitHTTPService::MicroBitHTTPService(BLEDevice &_ble) : ble(_ble) {
  GattCharacteristic urlCharacteristic(MicroBitHTTPServiceUrlUUID, (uint8_t*)&urlCharacteristicBuffer, URL_LENGTH, URL_LENGTH, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);
  GattCharacteristic *characteristics[] = {&urlCharacteristic};

  urlCharacteristicBuffer = (char*)malloc(URL_LENGTH);

  urlCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

  GattService service(MicroBitHTTPServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic*));
  ble.addService(service);

  urlCharacteristicHandle = urlCharacteristic.getValueHandle();

  ble.onDataWritten(this, &MicroBitHTTPService::onDataWritten);

}

void MicroBitHTTPService::onDataWritten(const GattWriteCallbackParams *params) {

}

void MicroBitHTTPService::updateURL(char* url) {
  strcpy(urlCharacteristicBuffer, url);
  ble.gattServer().write(urlCharacteristicHandle, (uint8_t*)&urlCharacteristicBuffer, sizeof(urlCharacteristicBuffer));
}
const uint8_t MicroBitHTTPServiceUUID[] = {
  0x13, 0x51, 0x63, 0x4a, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};

const uint8_t MicroBitHTTPServiceUrlUUID[] = {
  0x13, 0x51, 0x3f, 0x02, 0x09, 0xd1, 0x48, 0x46, 0x99, 0xb9, 0xee, 0x31, 0x12, 0xc3, 0xf5, 0x5b
};
