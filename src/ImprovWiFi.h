#ifndef _IMPROV_WIFI_H
#define _IMPROV_WIFI_H

#include "improv.h"

class ImprovWiFi {
public:
    ImprovWiFi(const char *firmwareName, const char *version, const char *chip, const char *deviceName);

    void loop();

    void setWiFiCallback(std::function<void(const char*, const char*)> callback);
    void setInfoCallback(std::function<void(const char*)> callback);
    void setDebugCallback(std::function<void(const char*)> callback);
private:
    void getAvailableWifiNetworks();
    void sendProvisioned(improv::Command responseTo);

    void set_state(improv::State state);
    void send_response(std::vector<uint8_t> &response);
    void set_error(improv::Error error);

    bool onCommandCallback(improv::ImprovCommand cmd);
    void onErrorCallback(improv::Error err);

    char msgBuf[16];
    uint8_t x_buffer[50];
    uint8_t x_position = 0;
    const char *firmwareName, *version, *chip, *deviceName;

    improv::Command pendingResponse;
    unsigned long pendingCmdReceived;

    std::function<void(const char*)> infoCallback;
    std::function<void(const char*)> debugCallback;
    std::function<void(const char*, const char*)> wifiCallback;
};

#endif