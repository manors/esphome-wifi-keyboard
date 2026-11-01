#pragma once
#include <esp_wifi.h>

extern "C" {
  esp_err_t __real_esp_wifi_set_config(wifi_interface_t interface, wifi_config_t *conf);

  esp_err_t __wrap_esp_wifi_set_config(wifi_interface_t interface, wifi_config_t *conf) {
    if (interface == WIFI_IF_STA && conf != nullptr) {
      conf->sta.listen_interval = 50;
    }
    return __real_esp_wifi_set_config(interface, conf);
  }
}