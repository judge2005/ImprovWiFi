#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <ImprovWiFiDevice.h>

Preferences prefs;

/**
 * So we have somwhere to store the hostname and device name.
 */
void initPrefs() {
    static bool initialized = false;
    if (initialized) {
        return; // Already ran once this boot; do nothing
    }

    if (prefs.begin("improv", false)) {
        if (!prefs.isKey("hostname")) {
            prefs.putString("hostname", "");
        }
        if (!prefs.isKey("devicename")) {
            prefs.putString("devicename", "whatever");
        }
        initialized = true;
    }
}

const char *getDeviceName() {
    initPrefs();
    static String devicename;
    devicename = prefs.getString("devicename");
    return devicename.c_str();
}

const char *getHostName() {
    initPrefs();
    static String hostname;
    hostname = prefs.getString("hostname");
    return hostname.c_str();
}

const char *getChipVariant() {
    return ESP.getChipModel();
}

/**
 * Subclass of ImprovWiFiDevice that implements the connectWiFi, getHostName, setHostName, getDeviceName,
 * and setDeviceName methods using the WiFi library and Preferences.
 */
class MyImprovWiFiDevice : public ImprovWiFiDevice {
public:
    MyImprovWiFiDevice(const char *firmwareName, const char *version, const char *chip, const char *deviceName)
        : ImprovWiFiDevice(firmwareName, version, chip, deviceName)
    {}

protected:
    /**
     * Override the connectWiFi method to use the WiFi library to connect to the specified SSID and password.
     * This method is called when the device receives a WIFI_SETTINGS command from the Improv protocol.
     */
    void connectWiFi(const char *ssid, const char *password) override {
        WiFi.disconnect();
        WiFi.begin(ssid, password);
    }

    /**
     * Override the getHostName method to retrieve the device's hostname from preferences.
     * This method is called when the device receives a GET_HOSTNAME command from the Improv protocol.
     */
    bool getHostName(String &hostName) override {
        hostName = ::getHostName();
        return true;
    }

    /**
     * Override the setHostName method to save the device's hostname to preferences.
     * This method is called when the device receives a SET_HOSTNAME command from the Improv protocol.
     */
    improv::Error setHostName(const char *hostName) override {
        if (prefs.putString("hostname", hostName) == 0) {
            return improv::Error::ERROR_UNKNOWN;
        }

        // A real implementation should also set the hostname in mDNS. That seems to require that
        // the device is restarted if it is to take effect.
        // For now, just set the hostname in WiFi and reconnect if already connected.
        WiFi.setHostname(hostName);
        if (WiFi.isConnected()) {
            WiFi.disconnect();
            WiFi.begin();
        }

        return improv::Error::ERROR_NONE;
    }

    /**
     * Override the getDeviceName method to retrieve the device's name from preferences.
     * This method is called when the device receives a GET_DEVICENAME command from the Improv protocol.
     */
    bool getDeviceName(String &deviceName) override {
        deviceName = ::getDeviceName();
        return true;
    }

    /**
     * Override the setDeviceName method to save the device's name to preferences.
     * This method is called when the device receives a SET_DEVICENAME command from the Improv protocol.
     */
    improv::Error setDeviceName(const char *deviceName) override {
        if (prefs.putString("devicename", deviceName) == 0) {
            return improv::Error::ERROR_UNKNOWN;
        }

        return improv::Error::ERROR_NONE;
    }
};

MyImprovWiFiDevice improvWiFiDevice("Test for ImprovWiFi", "1.0.0", getChipVariant(), getDeviceName());

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);

    // if a hostname is saved in preferences, set it as the device's hostname
    String savedHost = getHostName();
    if (savedHost.length() > 0) {
        WiFi.setHostname(savedHost.c_str());
    }
}

void loop() {
    // handle any incoming Improv commands and responses
    improvWiFiDevice.loop();
}
