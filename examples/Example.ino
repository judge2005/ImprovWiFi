#include <Arduino.h>
#include <WiFi.h>
#include <ImprovWiFi.h>

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

ImprovWiFi improvWiFi(manifest[0], manifest[1], manifest[2], manifest[3]);

void setWiFiCredentials(const char *ssid, const char *password) {
	WiFi.disconnect();
    WiFi.begin(ssid, password);
}

void setup() {
    improvWiFi.setWiFiCallback(setWiFiCredentials);
}

void loop() {
    improvWiFi.loop();
}