#include "BleSniffingModule.h"
#include "Default.h"
#include "MeshService.h"
#include "NodeDB.h"
#include "PowerFSM.h"
#include "configuration.h"
#include "main.h"
#include <Throttle.h>

BleSniffingModule *bleSniffingModule;

#define BLE_SNIFFING_INTERVAL 30*1000

typedef enum {
    BleSniffingVerdictDetected,
    BleSniffingVerdictSendState,
    BleSniffingVerdictNoop,
} BleSniffingTriggerVerdict;

typedef BleSniffingTriggerVerdict (*BleSniffingTriggerHandler)(bool prev, bool current);

static BleSniffingTriggerVerdict sniffing_trigger_logic_level(bool prev, bool current)
{
    return current ? BleSniffingVerdictDetected : BleSniffingVerdictNoop;
}

const static BleSniffingTriggerHandler handlers[] = {
    sniffing_trigger_logic_level,
};

int32_t BleSniffingModule::runOnce()
{
    if (firstTime) {
        firstTime = false;
        LOG_INFO("BLE Sniffing Module: init");
        
        return setStartDelay();
    }

    if (!Throttle::isWithinTimespanMs(lastSniffed, BLE_SNIFFING_INTERVAL)) {
        bool isDetected = hasSniffingEvent();
        BleSniffingTriggerVerdict verdict = handlers[0](wasDetected, isDetected);
        wasDetected = isDetected;
        lastSniffed = millis();
        /*
        switch (verdict) {
        case BleSniffingVerdictDetected:
            sendSniffingMessage();
            return BLE_SNIFFING_INTERVAL;
        case BleSniffingVerdictSendState:
            sendCurrentStateMessage(isDetected);
            return BLE_SNIFFING_INTERVAL;
        case BleSniffingVerdictNoop:

            return BLE_SNIFFING_INTERVAL;
        }*/

        
    }
    return BLE_SNIFFING_INTERVAL;
}

void BleSniffingModule::sendSniffingMessage()
{
    LOG_DEBUG("Sniffing event observed. Send message");
    char *message = new char[40];
    sprintf(message, "BLE sniffing detected");
    meshtastic_MeshPacket *p = allocDataPacket();
    p->want_ack = false;
    p->decoded.payload.size = strlen(message);
    memcpy(p->decoded.payload.bytes, message, p->decoded.payload.size);
    lastSniffed = millis();
    if (!channels.isDefaultChannel(0)) {
        LOG_INFO("Send message id=%d, dest=%x, msg=%.*s", p->id, p->to, p->decoded.payload.size, p->decoded.payload.bytes);
        service->sendToMesh(p);
    } else {
        LOG_ERROR("Message not allowed on Public channel");
    }
    delete[] message;
}

void BleSniffingModule::sendCurrentStateMessage(bool state)
{
    char *message = new char[40];
    sprintf(message, "BLE sniffing state: %i", state);
    meshtastic_MeshPacket *p = allocDataPacket();
    p->want_ack = false;
    p->decoded.payload.size = strlen(message);
    memcpy(p->decoded.payload.bytes, message, p->decoded.payload.size);
    lastSniffed = millis();
    if (!channels.isDefaultChannel(0)) {
        LOG_INFO("Send message id=%d, dest=%x, msg=%.*s", p->id, p->to, p->decoded.payload.size, p->decoded.payload.bytes);
        service->sendToMesh(p);
    } else {
        LOG_ERROR("Message not allowed on Public channel");
    }
    delete[] message;
}

bool BleSniffingModule::hasSniffingEvent()
{
    //#ifdef ARCH_NRF52
        LOG_INFO("Before NRF52Bluetooth");
        //nrf52Bluetooth = new NRF52Bluetooth();
        // If not yet set-up
        if (!nrf52Bluetooth) {
            LOG_DEBUG("Init NRF52 Bluetooth");
            nrf52Bluetooth = new NRF52Bluetooth();
            nrf52Bluetooth->setupSniffing(10);

            // We delay brownout init until after BLE because BLE starts soft device
           // initBrownout();
        }
        // Already setup, apparently
        else
            LOG_DEBUG("Already setup NRF52 Bluetooth");
            nrf52Bluetooth->setupSniffing(100);
    //#endif

    lastSniffed = millis();
    // Implémentez la logique de détection de sniffing BLE ici
    return 2;
}