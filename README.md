# Overview

This library simplifies adding the [Improv](https://improv-wifi.com) [serial protocol](https://improv-wifi.comserial/) to ESP32 projects. When used with [ESP Web Tools](https://github.io), it can:

1. Return firmware and device information
2. Set up the Wi-Fi connection
3. Return the device's application URL
4. Optionally allow the device hostname and device name to be read or changed
5. Report network connectivity status, supported interfaces, and reachable URLs

The design is non-blocking while waiting for a Wi-Fi connection and can be used with any Wi-Fi stack, including Wi-Fi managers.

ESP Web Tools lets a browser that supports serial port access (for example Chrome, Edge, or Opera) flash an ESP32 and use Improv to discover firmware and provision a Wi-Fi connection.

## ImprovWiFi

`ImprovWiFi` is the original class that implements all the functions to add ESP Web Tools WiFi provisioning to your
ESP32 device:

1. Declare an `ImprovWiFi` instance with a description of the device, the version, the chip type and a device name.
2. Define a Wi-Fi callback with `setWiFiCallback()`.
3. Optionally define info and debug callbacks with `setInfoCallback()` and `setDebugCallback()` to print info and
debug messages if your device has some way of displaying them that isn't the serial port that improv is using!
4. Call `loop()` regularly. It never waits for a Wi-Fi connection and is safe for the Arduino loop to continue running.

The Wi-Fi callback is responsible for starting the connection asynchronously. The library detects `WiFi.status() == WL_CONNECTED`, sends the provisioning response, and reports `http://<device-ip>/`.

Look at examples/Example.ino for some example code.

## ImprovWiFiDevice

`ImprovWiFiDevice` inherits from `ImprovWiFi` and adds support for reading and setting a hostname and device name, as well as reporting network state (`GET_NETWORK_STATE`, `0x07`), through Improv commands. This changes the way that callbacks are handled. It has virtual methods for Wi-Fi, device name, hostname, and network state reporting so you should create a subclass of `ImprovWiFiDevice` and override the specific methods you need for your implementation:

1. Define a subclass of `ImprovWiFiDevice` - overriding only those methods you need for your device/implementation.
2. Declare an instance of your subclass making sure you pass the device description, version, chip type and device name
to the `ImprovWiFiDevice` constructor
3. Call `loop()` regularly. It never waits for a Wi-Fi connection and is safe for the Arduino loop to continue running.

- `connectWiFi(const char *ssid, const char *password)` to connect your device to Wi-Fi
- `getHostName(String &hostName)` and `setHostName(const char *hostName)` to get/set your hostname
- `getDeviceName(String &deviceName)` and `setDeviceName(const char *deviceName)` to get/set your device name
- `getNetworkState()` if you want to add support for other network types (the default implementation reports Wi-Fi state).
- `getNetworkUrls()` if you want to return more URLs - the default reads the Wi-Fi IP address if it is online.
- `onInfo(const char *msg)` if you can output info level messages without using the same serial device as Improv.
- `onDebug(const char *msg)` if you can output debug level messages without using the same serial device as Improv.

`getHostName()` and `getDeviceName()` should return `true` when they supply a value. `set*()` should return `improv::Error::ERROR_NONE` on success or an appropriate Improv error on failure. Hostnames are validated before `setHostName()` is called; names may contain letters, numbers, and hyphens, but may not start or end with a hyphen.

If a property getter is not implemented, `ImprovWiFiDevice` falls back to the constructor's default device name for `GET_DEVICE_INFO`, and reports unknown-command errors for unsupported property operations.

By default, `getNetworkState()` reports `NETWORK_SUPPORTS_WIFI` (plus `NETWORK_IS_ONLINE` when `WiFi.status() == WL_CONNECTED`), and `getNetworkUrls()` returns `http://<device-ip>/` when connected. Override them if your device supports additional interfaces (such as Ethernet) or exposes custom URLs (such as an mDNS `.local` address).

The `connectWiFi()` method is responsible for starting the connection asynchronously. The library detects `WiFi.status() == WL_CONNECTED`, sends the provisioning response, and reports `http://<device-ip>/`.

Look at examples/WiFiDeviceExample.ino for some example code.

Writing application messages directly to the serial port can interfere with Improv messaging. Prefer the optional info and debug callbacks for status reporting, or use another output mechanism such as Wi-Fi or a display.
