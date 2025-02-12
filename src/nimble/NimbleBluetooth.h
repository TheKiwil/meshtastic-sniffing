#pragma once
#include "BluetoothCommon.h"
#include <NimBLEDevice.h>

class NimbleBluetooth : BluetoothApi
{
  public:
    void setup();
    void shutdown();
    void deinit();
    void clearBonds();
    bool isActive();
    bool isConnected();
    int getRssi();
    void sendLog(const uint8_t *logMessage, size_t length);
    
    // BLE Scanning functions
    void setupSniffing();
    void startSniffing(uint16_t delayS);
    void getBeaconsMacAddr(char* buffer, size_t bufferSize, size_t count);

  private:
    NimBLEScan* pBLEScan;
    void setupService();
    void startAdvertising();
};

void setBluetoothEnable(bool enable);
void clearNVS();
