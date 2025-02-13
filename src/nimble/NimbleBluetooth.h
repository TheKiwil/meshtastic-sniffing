#pragma once
#include "BluetoothCommon.h"
#include <NimBLEDevice.h>

// Maximum number of beacons to remember
const size_t MAX_BEACONS = 500;

struct BeaconEntry
{
  NimBLEAddress addr;
  uint32_t lastSeen;
  char mac_addr[18];
  int8_t rssi;
};
struct CompareBeaconRSSI
{
  bool operator()(const BeaconEntry &a, const BeaconEntry &b)
  {
    return a.rssi > b.rssi; // Sort descending (strongest RSSI first)
  }
};

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
  bool startSniffing(uint16_t delayS);
  void getBeaconsMacAddr(char *buffer, size_t bufferSize, size_t count);

private:
  NimBLEScan *pBLEScan;
  static std::vector<BeaconEntry> seenBeacons;

  void setupService();
  void startAdvertising();
};

void setBluetoothEnable(bool enable);
void clearNVS();
