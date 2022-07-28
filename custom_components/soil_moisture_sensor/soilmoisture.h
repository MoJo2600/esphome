#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace soil_moisture_sensor {

struct ParseResult {
  optional<float> temperature;
  optional<float> moisture;
  optional<float> battery_level;
  // int raw_offset;
};


class SoilMoistureSensor : public sensor::Sensor, public PollingComponent {
 public:
  // void set_address(uint64_t address) { address_ = address; };

  // void dump_config() override;
  // float get_setup_priority() const override { return setup_priority::DATA; }
  void set_temperature(sensor::Sensor *temperature) { temperature_ = temperature; }
  void set_moisture(sensor::Sensor *moisture) { moisture_ = moisture; }
  void set_battery_level(sensor::Sensor *battery_level) { battery_level_ = battery_level; }
  void set_battery_level_raw(sensor::Sensor *battery_level_raw) { battery_level_raw_ = battery_level_raw; }
  void set_adc_dry_at_3v(int adc_dry_at_3v) { adc_dry_at_3v_ = adc_dry_at_3v; }

  /// Update values.
  void update() override;
  /// Setup
  void setup() override;
  void dump_config() override;
  /// `HARDWARE_LATE` setup priority.
  float get_setup_priority() const override;
  // void set_pin(InternalGPIOPin *pin) { this->pin_ = pin; }
  // void set_output_raw(bool output_raw) { output_raw_ = output_raw; }
  // float sample() override;

#ifdef USE_ESP8266
  std::string unique_id() override;
#endif

 protected:
  uint64_t address_;
  sensor::Sensor *temperature_{nullptr};
  sensor::Sensor *moisture_{nullptr};
  sensor::Sensor *battery_level_{nullptr};
  sensor::Sensor *battery_level_raw_{nullptr};
  int adc_dry_at_3v_;

  // optional<ParseResult> parse_header_(const esp32_ble_tracker::ServiceData &service_data);
  // bool parse_message_(const std::vector<uint8_t> &message, ParseResult &result);
  // bool report_results_(const optional<ParseResult> &result, const std::string &address);

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
  float readTemperature();
};

}  // namespace soil_moisture_sensor
}  // namespace esphome
