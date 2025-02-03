#pragma once

#include "BluetoothCommon.h"
#include <Arduino.h>
#include "ble_gap.h"

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

    void setupSniffing(uint16_t delayS);

  private:
    volatile bool scanComplete = false;
    unsigned long scanStartTime = 0;
    uint16_t scanDuration = 0;
    
    static void scan_callback(ble_gap_evt_adv_report_t* report);
    static void onConnectionSecured(uint16_t conn_handle);
    static bool onPairingPasskey(uint16_t conn_handle, uint8_t const passkey[6], bool match_request);
    static void onPairingCompleted(uint16_t conn_handle, uint8_t auth_status);
};