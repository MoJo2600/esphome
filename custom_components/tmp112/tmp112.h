#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace tmp112 {

class Tmp112 : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  void setup() override;
  void update() override;
  void dump_config() override;

#ifdef USE_ESP8266
  std::string unique_id() override;
#endif

};

}  // namespace tmp112
}  // namespace esphome