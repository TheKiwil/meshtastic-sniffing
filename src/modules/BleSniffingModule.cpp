#include "BleSniffingModule.h"
#include "Default.h"
#include "MeshService.h"
#include "NodeDB.h"
#include "PowerFSM.h"
#include "configuration.h"
#include "main.h"
#include <Throttle.h>

#if HAS_WIFI
#include "mesh/wifi/WiFiAPClient.h"
#endif

BleSniffingModule *bleSniffingModule;

#define BLE_SNIFFING_INTERVAL 120 * 1000
#define BLE_SNIFFING_MAX_BEACONS 3
#define BLE_SNIFFING_DURATION 5 // second

typedef enum
{
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
    if (firstTime)
    {
        firstTime = false;
        LOG_INFO("BLE Sniffing Module: init");

        return setStartDelay();
    }

    if (!Throttle::isWithinTimespanMs(lastSniffed, BLE_SNIFFING_INTERVAL))
    {
        if (hasSniffingEvent())
        {
            LOG_DEBUG("Sniffing event detected");
            sendSniffingMessage();
        }
        else
        {
            lastSniffed = millis();
            LOG_DEBUG("No sniffing event detected");
        }
    }
    return BLE_SNIFFING_INTERVAL;
}

void BleSniffingModule::sendSniffingMessage()
{
    LOG_DEBUG("Sniffing event observed. Send message");
    char *bleBeacons = new char[BLE_SNIFFING_MAX_BEACONS * 14];
    memset(bleBeacons, 0, BLE_SNIFFING_MAX_BEACONS * 14);

#ifdef ARCH_ESP32
    nimbleBluetooth->getBeaconsMacAddr(bleBeacons, (BLE_SNIFFING_MAX_BEACONS * 14), BLE_SNIFFING_MAX_BEACONS);
#endif

#ifdef ARCH_NRF52
    nrf52Bluetooth->getBeaconsMacAddr(bleBeacons, (BLE_SNIFFING_MAX_BEACONS * 14), BLE_SNIFFING_MAX_BEACONS);
#endif

    // Add "BLE:" prefix to the message
    char *message = new char[strlen(bleBeacons) + 5];
    strcpy(message, "BLE:");
    strcat(message, bleBeacons);

    meshtastic_MeshPacket *p = allocDataPacket();
    p->to = NODENUM_BROADCAST;
    p->priority = meshtastic_MeshPacket_Priority_RELIABLE;
    p->want_ack = false;
    p->decoded.portnum = meshtastic_PortNum_TEXT_MESSAGE_APP;
    p->decoded.payload.size = strlen(message);
    memcpy(p->decoded.payload.bytes, message, p->decoded.payload.size);
    lastSniffed = millis();
    service->sendToMesh(p, RX_SRC_LOCAL, true);

    delete[] message;
    delete[] bleBeacons;
}

bool BleSniffingModule::hasSniffingEvent()
{
    bool detected = false;
#ifdef ARCH_ESP32
#if HAS_WIFI
    if (!isWifiAvailable())
    {
#endif
        if (!nimbleBluetooth)
            nimbleBluetooth = new NimbleBluetooth();

        detected = nimbleBluetooth->startSniffing(BLE_SNIFFING_DURATION);
#if HAS_WIFI
    }
    else
    {
        LOG_DEBUG("Wifi is available, not compatible with BLE sniffing");
    }
#endif
#endif

#ifdef ARCH_NRF52
    // If not yet set-up
    if (!nrf52Bluetooth)
    {
        LOG_DEBUG("Init NRF52 Bluetooth");
        nrf52Bluetooth = new NRF52Bluetooth();
        nrf52Bluetooth->setupSniffing();
        detected = nrf52Bluetooth->startSniffing(BLE_SNIFFING_DURATION);
    }
    else
    { // Already setup, apparently
        LOG_DEBUG("Already setup NRF52 Bluetooth");
        detected = nrf52Bluetooth->startSniffing(BLE_SNIFFING_DURATION);
    }
#endif

    return detected;
}