#include "esphome/core/log.h"
#include "tmp112.h"

namespace esphome {
namespace tmp112 {

static const char *TAG = "tmp112.sensor";

void Tmp112::setup(){
  if (!this->write_byte(0x01, 0)) {
    ESP_LOGE(TAG, "Communication with TMP112 failed!");
    this->mark_failed();
    return;
  }

  if (!this->write_byte(0x60, 0)) {
    ESP_LOGE(TAG, "Communication with TMP112 failed!");
    this->mark_failed();
    return;
  }
}

void Tmp112::update(){
    this->write_byte(0x00, 0);
    uint8_t data[2];
    this->read_bytes(0x00, data, 2);
    int rawtmp = data[0] << 8 | data[1];
    int value = rawtmp >> 4;
    float temp = value * 0.0625;
    ESP_LOGI(TAG, "Temperature: %f", temp);
    this->publish_state(temp);
}

void Tmp112::dump_config(){

}

#ifdef USE_ESP8266
std::string Tmp112::unique_id() { return get_mac_address() + "-temperature"; }
#endif


}  // namespace tmp112
}  // namespace esphome