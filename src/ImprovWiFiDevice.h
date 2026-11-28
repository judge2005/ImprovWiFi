#ifndef _IMPROV_WIFI_DEVICE_H
#define _IMPROV_WIFI_DEVICE_H

#include "ImprovWiFi.h"

class ImprovWiFiDevice : public ImprovWiFi {
public:
    using ImprovWiFi::ImprovWiFi;

    static bool isValidHostName(const char *hostName);

protected:
    bool onCommand(const improv::ImprovCommand &cmd) override;

    virtual bool getHostName(String &hostName);
    virtual improv::Error setHostName(const char *hostName);

    virtual bool getDeviceName(String &deviceName);
    virtual improv::Error setDeviceName(const char *deviceName);

    virtual uint8_t getNetworkState();
    virtual std::vector<std::string> getNetworkUrls();

private:
    bool handleProperty(
        const improv::ImprovCommand &cmd,
        bool (ImprovWiFiDevice::*getter)(String &),
        improv::Error (ImprovWiFiDevice::*setter)(const char *));
};

#endif
