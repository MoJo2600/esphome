#pragma once

#include "Arduino.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace ap_soil_moisture_sensor {

class SoilMoistureSensor : public sensor::Sensor, public PollingComponent {
 public:
  // void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  void set_moisture(sensor::Sensor *moisture) { moisture_ = moisture; }
  void set_moisture_raw(sensor::Sensor *moisture_raw) { moisture_raw_ = moisture_raw; }
  void set_battery_level(sensor::Sensor *battery_level) { battery_level_ = battery_level; }
  void set_battery_level_raw(sensor::Sensor *battery_level_raw) { battery_level_raw_ = battery_level_raw; }
  void set_adc_dry_at_3v(int adc_dry_at_3v) { adc_dry_at_3v_ = adc_dry_at_3v; }
  void set_adc_wet_at_3v(int adc_wet_at_3v) { adc_wet_at_3v_ = adc_wet_at_3v; }

  /// Update values.
  void update() override;
  /// Setup
  void setup() override;
  void dump_config() override;
  // float get_setup_priority() const override;

#ifdef USE_ESP8266
  std::string unique_id() override;
#endif

 protected:
  uint64_t address_;
  sensor::Sensor *moisture_{nullptr};
  sensor::Sensor *moisture_raw_{nullptr};
  sensor::Sensor *battery_level_{nullptr};
  sensor::Sensor *battery_level_raw_{nullptr};
  int adc_dry_at_3v_;
  int adc_wet_at_3v_;

 private:
  /*
  * Function: readSensor
  * --------------------
  * Reads the sensor connected to pin A0
  * Since the moisture sensor and the battery voltage are
  * connected to the same input pin, this function can be used
  * for both tasks.
  */
  int readSensor();
};

}  // namespace soil_moisture_sensor
}  // namespace esphome
