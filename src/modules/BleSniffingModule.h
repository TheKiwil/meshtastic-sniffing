#pragma once
#include "SinglePortModule.h"

class BleSniffingModule : public SinglePortModule, private concurrency::OSThread
{
  public:
    BleSniffingModule() : SinglePortModule("sniffing", meshtastic_PortNum_SNIFFING_APP), OSThread("SniffingBLE")
    {
    }

  protected:
    virtual int32_t runOnce() override;

  private:
    bool firstTime = true;
    uint32_t lastSniffed = 0;
    bool wasDetected = false;
    void sendSniffingMessage();
    bool hasSniffingEvent();
};

extern BleSniffingModule *bleSniffingModule;