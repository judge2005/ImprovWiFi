# Overview

This library simplies adding the [improv](https://www.improv-wifi.com/) [serial protocol](https://www.improv-wifi.com/serial/) to your ESP32 projects. When used with [esp web tools](https://esphome.github.io/esp-web-tools/) it fullfills three purposes:

1. Returns the manifest data when asked
2. Sets up the wifi connection
3. Returns the application url

The design is completely non-blocking, even when 'waiting' for a wifi connection, and can be used with any WiFi stack, including WiFi managers.

For those that don't know, esp web tools allows an ESP32 device to be flashed using a web browser that supports serial port access (basically Chrome, Edge and Opera). Improv allows esp web tools to query what firmware is already flashed and to set up a WiFi connection on the device.

## How to Use

1. Declare an instance of the class, constructed with the manifest information
2. Define a callback to connect to WiFi
3. Optionally define a callback to display info messages
4. Optionally define a callback to display debug messages
5. Call the loop() method every so often. This method never blocks.

Obviously writing messages to the serial port will interefere with the improv messaging, but there are sometimes other ways to get that information in front of a person.

