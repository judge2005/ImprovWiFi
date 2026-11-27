#include <cctype>
#include <cstring>
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

    default:
      return ImprovWiFi::onCommand(cmd);
  }
}
