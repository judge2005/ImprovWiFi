# Overview

This library simplifies adding the [Improv](https://www.improv-wifi.com/) [serial protocol](https://www.improv-wifi.com/serial/) to ESP32 projects. When used with [ESP Web Tools](https://esphome.github.io/esp-web-tools/), it can:

1. Return firmware and device information
2. Set up the Wi-Fi connection
3. Return the device's application URL
4. Optionally allow the device hostname and device name to be read or changed

The design is non-blocking while waiting for a Wi-Fi connection and can be used with any Wi-Fi stack, including Wi-Fi managers.

ESP Web Tools lets a browser that supports serial port access (for example Chrome, Edge, or Opera) flash an ESP32 and use Improv to discover firmware and provision a Wi-Fi connection.

## ImprovWiFi

`ImprovWiFi` provides the base serial protocol implementation. Construct it with the firmware name, firmware version, hardware chip, and default device name.

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <ImprovWiFi.h>

ImprovWiFi improvWiFi(
    "Test for ImprovWiFi",
    "1.0.0",
    "ESP32",
    "Whatever"
);

void setWiFiCredentials(const char *ssid, const char *password) {
    WiFi.disconnect();
    WiFi.begin(ssid, password);
}

void setup() {
    Serial.begin(115200);
    improvWiFi.setWiFiCallback(setWiFiCredentials);
}

void loop() {
    improvWiFi.loop();
}
```

### Base class setup

1. Declare an `ImprovWiFi` instance with the manifest information.
2. Define a Wi-Fi callback with `setWiFiCallback()`.
3. Optionally define info and debug callbacks with `setInfoCallback()` and `setDebugCallback()`.
4. Call `loop()` regularly. It never waits for a Wi-Fi connection and is safe for the Arduino loop to continue running.

The Wi-Fi callback is responsible for starting the connection asynchronously. The library detects `WiFi.status() == WL_CONNECTED`, sends the provisioning response, and reports `http://<device-ip>/`.

## ImprovWiFiDevice

`ImprovWiFiDevice` inherits from `ImprovWiFi` and adds support for reading and setting a hostname and device name through Improv commands. The methods are virtual, so subclass it and persist or apply values as needed.

```cpp
#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <ImprovWiFiDevice.h>

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
        return true;
    }

    improv::Error setHostName(const char *hostName) override {
        if (prefs.putString("hostname", hostName) == 0) {
            return improv::Error::ERROR_UNKNOWN;
        }
        WiFi.setHostname(hostName);
        return improv::Error::ERROR_NONE;
    }

    bool getDeviceName(String &deviceName) override {
        deviceName = prefs.getString("device-name", "");
        return deviceName.length() > 0;
    }

    improv::Error setDeviceName(const char *deviceName) override {
        if (prefs.putString("device-name", deviceName) == 0) {
            return improv::Error::ERROR_UNKNOWN;
        }
        return improv::Error::ERROR_NONE;
    }

private:
    Preferences prefs;
};

MyImprovWiFiDevice device("Test for ImprovWiFi", "1.0.0", "ESP32", "Whatever");

void setup() {
    Serial.begin(115200);
    device.begin();
}

void loop() {
    device.loop();
}
```

For `ImprovWiFiDevice`, implement the following protected methods:

- `getHostName(String &hostName)` and `setHostName(const char *hostName)`
- `getDeviceName(String &deviceName)` and `setDeviceName(const char *deviceName)`

`get*()` should return `true` when it supplies a value. `set*()` should return `improv::Error::ERROR_NONE` on success or an appropriate Improv error on failure. Hostnames are validated before `setHostName()` is called; names may contain letters, numbers, and hyphens, but may not start or end with a hyphen.

If a property getter is not implemented, `ImprovWiFiDevice` falls back to the constructor's default device name for `GET_DEVICE_INFO`, and reports unknown-command errors for unsupported property operations.

Writing application messages directly to the serial port can interfere with Improv messaging. Prefer the optional info and debug callbacks for status reporting, or use another output mechanism such as Wi-Fi or a display.

