#pragma once
#include "SinglePortModule.h"

class BleSniffingModule : private concurrency::OSThread
{
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