#pragma once

#include "BluetoothCommon.h"
#include <Arduino.h>
#include <vector>
#include "ble_gap.h"

// Maximum number of beacons to remember
const size_t MAX_BEACONS = 500;

struct BeaconEntry {
    ble_gap_addr_t addr;
    uint32_t lastSeen;
    char mac_addr[18];
    int8_t rssi;
};
struct CompareBeaconRSSI {
    bool operator()(const BeaconEntry& a, const BeaconEntry& b) {
        return a.rssi > b.rssi; // Sort descending (strongest RSSI first)
    }
};
class NRF52Bluetooth : BluetoothApi
{
  public:
    void setup();
    void shutdown();
    void startDisabled();
    void resumeAdvertising();
    void clearBonds();
    bool isConnected();
    int getRssi();
    void sendLog(const uint8_t *logMessage, size_t length);
        
    // Add scanning (sniffing) function
    void setupSniffing();
    bool startSniffing(uint16_t delayS);
    void getBeaconsMacAddr(char* buffer, size_t bufferSize, size_t count);

  private:
    static uint32_t numberOfBeacon;
    static std::vector<BeaconEntry> seenBeacons;

    static void scan_callback(ble_gap_evt_adv_report_t* report);
    static void onConnectionSecured(uint16_t conn_handle);
    static bool onPairingPasskey(uint16_t conn_handle, uint8_t const passkey[6], bool match_request);
    static void onPairingCompleted(uint16_t conn_handle, uint8_t auth_status);
};