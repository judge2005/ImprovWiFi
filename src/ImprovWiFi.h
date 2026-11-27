#ifndef _IMPROV_WIFI_H
#define _IMPROV_WIFI_H

#include "improv.h"

class ImprovWiFi {
public:
    ImprovWiFi(const char *firmwareName, const char *version, const char *chip, const char *deviceName);
    virtual ~ImprovWiFi() = default;

    void loop();

    void setWiFiCallback(std::function<void(const char*, const char*)> callback);
    void setInfoCallback(std::function<void(const char*)> callback);
    void setDebugCallback(std::function<void(const char*)> callback);

protected:
    virtual void connectWiFi(const char *ssid, const char *password);
    virtual void onInfo(const char *msg);
    virtual void onDebug(const char *msg);
    virtual bool onCommand(const improv::ImprovCommand &cmd);

    void sendDeviceInfo(const char *name);
    void set_state(improv::State state);
    void send_response(std::vector<uint8_t> &response);
    void set_error(improv::Error error);

    String deviceName;

private:
    void getAvailableWifiNetworks();
    void sendProvisioned(improv::Command responseTo);

    bool onCommandCallback(improv::ImprovCommand cmd);
    void onErrorCallback(improv::Error err);

    char msgBuf[16];
    uint8_t x_buffer[50];
    uint8_t x_position = 0;
    const char *firmwareName, *version, *chip;

    improv::Command pendingResponse;
    unsigned long pendingCmdReceived;

    std::function<void(const char*)> infoCallback;
    std::function<void(const char*)> debugCallback;
    std::function<void(const char*, const char*)> wifiCallback;
};

#endif
