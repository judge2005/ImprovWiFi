#include <cctype>
#include <cstring>
#include <WiFi.h>
#include "ImprovWiFiDevice.h"

bool ImprovWiFiDevice::isValidHostName(const char *hostName) {
  if (hostName == nullptr) {
    return false;
  }

  size_t len = std::strlen(hostName);
  if (len == 0 || len > 255) {
    return false;
  }

  for (size_t i = 0; i < len; ++i) {
    unsigned char ch = static_cast<unsigned char>(hostName[i]);
    if (!std::isalnum(ch) && ch != '-') {
      return false;
    }
    if ((i == 0 || i == len - 1) && ch == '-') {
      return false;
    }
  }

  return true;
}

bool ImprovWiFiDevice::getHostName(String &hostName) {
  return false;
}

improv::Error ImprovWiFiDevice::setHostName(const char *hostName) {
  return improv::Error::ERROR_UNKNOWN_RPC;
}

bool ImprovWiFiDevice::getDeviceName(String &deviceName) {
  return false;
}

improv::Error ImprovWiFiDevice::setDeviceName(const char *deviceName) {
  return improv::Error::ERROR_UNKNOWN_RPC;
}

uint8_t ImprovWiFiDevice::getNetworkState() {
  uint8_t state = improv::NetworkState::NETWORK_SUPPORTS_WIFI;
  if (WiFi.status() == WL_CONNECTED) {
    state |= improv::NetworkState::NETWORK_IS_ONLINE;
  }
  return state;
}

std::vector<std::string> ImprovWiFiDevice::getNetworkUrls() {
  std::vector<std::string> urls;
  if (WiFi.status() == WL_CONNECTED) {
    urls.push_back(getLocalIPUrl());
  }
  return urls;
}

bool ImprovWiFiDevice::handleProperty(
    const improv::ImprovCommand &cmd,
    bool (ImprovWiFiDevice::*getter)(String &),
    improv::Error (ImprovWiFiDevice::*setter)(const char *)) {
  if (!cmd.payload.empty()) {
    improv::Error err = (this->*setter)(cmd.payload.c_str());
    if (err != improv::Error::ERROR_NONE) {
      set_error(err);
      return false;
    }
  }

  String value;
  if (!(this->*getter)(value)) {
    if (cmd.payload.empty()) {
      set_error(improv::Error::ERROR_UNKNOWN_RPC);
      return false;
    }
    value = cmd.payload.c_str();
  }

  std::vector<uint8_t> data = improv::build_rpc_response(cmd.command, std::vector<String>{value}, false);
  send_response(data);
  return true;
}

bool ImprovWiFiDevice::onCommand(const improv::ImprovCommand &cmd) {
  switch (cmd.command) {
    case improv::Command::GET_DEVICE_INFO: {
      String name;
      if (!getDeviceName(name)) {
        name = deviceName;
      }
      sendDeviceInfo(name.c_str());
      return true;
    }

    case improv::Command::HOSTNAME:
      if (!cmd.payload.empty() && !isValidHostName(cmd.payload.c_str())) {
        set_error(improv::Error::ERROR_BAD_HOSTNAME);
        return false;
      }
      return handleProperty(cmd, &ImprovWiFiDevice::getHostName, &ImprovWiFiDevice::setHostName);

    case improv::Command::DEVICE_NAME:
      return handleProperty(cmd, &ImprovWiFiDevice::getDeviceName, &ImprovWiFiDevice::setDeviceName);

    case improv::Command::GET_NETWORK_STATE: {
      uint8_t state = getNetworkState();
      std::vector<std::string> payload = {std::to_string(state)};

      if (state & improv::NetworkState::NETWORK_IS_ONLINE) {
        std::vector<std::string> urls = getNetworkUrls();
        payload.insert(payload.end(), urls.begin(), urls.end());
      }

      std::vector<uint8_t> data = improv::build_rpc_response(
          improv::Command::GET_NETWORK_STATE, payload, false);
      send_response(data);
      return true;
    }

    default:
      return ImprovWiFi::onCommand(cmd);
  }
}
