#pragma once
#include "SinglePortModule.h"

class BleSniffingModule : public SinglePortModule, private concurrency::OSThread
{
  public:
    BleSniffingModule() : SinglePortModule("ble_sniffing", meshtastic_PortNum_BLE_SNIFFING_APP), OSThread("BleSniffing")
    {
    }

  protected:
    virtual int32_t runOnce() override;

  private:
    bool firstTime = true;
    uint32_t lastSniffed = 0;
    bool wasDetected = false;
    void sendSniffingMessage();
    void sendCurrentStateMessage(bool state);
    bool hasSniffingEvent();
};

extern BleSniffingModule *bleSniffingModule;