#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <ImprovWiFiDevice.h>

// Should match what is in example-manifest.json
const char *manifest[] = {
    // Firmware name
    "Test for ImprovWiFi",
    // Firmware version
    "1.0.0",
    // Hardware chip/variant
    "ESP32",
    // Device name
    "Whatever"
};

class MyImprovWiFiDevice : public ImprovWiFiDevice {
public:
    using ImprovWiFiDevice::ImprovWiFiDevice;

    void begin() {
        prefs.begin("improv", false);
        WiFi.mode(WIFI_STA);

        String savedHost = prefs.getString("hostname", "");
        if (savedHost.length() > 0) {
            WiFi.setHostname(savedHost.c_str());
        }
    }

protected:
    void connectWiFi(const char *ssid, const char *password) override {
        WiFi.disconnect();
        WiFi.begin(ssid, password);
    }

    bool getHostName(String &hostName) override {
        hostName = prefs.getString("hostname", "");
        if (hostName.length() == 0) {
            const char *current = WiFi.getHostname();
            hostName = current ? current : "";
        }
        return true;
    }

    improv::Error setHostName(const char *hostName) override {
        if (prefs.putString("hostname", hostName) == 0) {
            return improv::Error::ERROR_UNKNOWN;
        }
        WiFi.setHostname(hostName);
        return improv::Error::ERROR_NONE;
    }

private:
    Preferences prefs;
};

MyImprovWiFiDevice improvWiFiDevice(manifest[0], manifest[1], manifest[2], manifest[3]);

void setup() {
    Serial.begin(115200);
    improvWiFiDevice.begin();
}

void loop() {
    improvWiFiDevice.loop();
}
