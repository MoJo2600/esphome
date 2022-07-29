#include "soilmoisture.h"
#include "esphome/core/log.h"
#include "Arduino.h"
#include "Wire.h"

namespace esphome {
namespace ap_soil_moisture_sensor {

static const char *const TAG = "soilmoisture";

// const int TMP_ADDR = 0x48;
const int PIN_SWITCH = D8;
const int PIN_CLK    = D5;

/*
* Function: nonBlockingDelay
* --------------------------
* A delay function that will not block the cpu like delay() does.
*/
void nonBlockingDelay(int waitmillis) {
  int time_now = millis();
  while(millis() < time_now + waitmillis) {
      //wait without stopping the cpu
      yield();
  }
}

void SoilMoistureSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "AP SOIL MOISTURE SENSOR:");
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Battery Level Sensor", this->battery_level_);
  LOG_SENSOR("  ", "Battery Level Sensor RAW", this->battery_level_raw_);
  LOG_SENSOR("  ", "Moisture Sensor", this->moisture_);
  LOG_SENSOR("  ", "Moisture Sensor RAW", this->moisture_raw_);
}

void SoilMoistureSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SoilMoistureSensor '%s'...", this->get_name().c_str());
  // Setup switch pin for battery/sensor connection to ADC pin
  pinMode(PIN_SWITCH, OUTPUT);
  digitalWrite(PIN_SWITCH, LOW);
  // Setup PWM signal for capacitive sensor
  analogWriteFreq(60000);
  analogWrite(PIN_CLK, 20);
}

int SoilMoistureSensor::readSensor() {
  int total = 0;
  int adcVal = 0;
  int ret = 0;
  int sampleCount = 3;

  for(int i = 0; i < sampleCount; i++){
    adcVal = analogRead(A0);
    total += adcVal;
    nonBlockingDelay(50);
  }

  ret = int((float)total / (float)sampleCount);

  return ret;
}

void SoilMoistureSensor::update() {
  int vcc_reading_3V = 958; 
  int vcc_reading_range = 166;

  ESP_LOGD("soilmoisturesensor", "sensor_dry_adc value is %d", this->adc_dry_at_3v_);
  ESP_LOGD("soilmoisturesensor", "sensor_wet_adc value is %d", this->adc_wet_at_3v_);

  // Read battery level
  digitalWrite(PIN_SWITCH, LOW);
  nonBlockingDelay(150);

  int battery_level_raw = this->readSensor();
  ESP_LOGD(TAG, "Battery ADC reading is %d", battery_level_raw);

  if (this->battery_level_ != nullptr)
    this->battery_level_raw_->publish_state(battery_level_raw);

  int battery_level = map(battery_level_raw, vcc_reading_3V-vcc_reading_range, vcc_reading_3V, 0, 100);  
  if (battery_level > 100) battery_level = 100;
  if (battery_level < 0) battery_level = 0;

  ESP_LOGI("soilmoisturesensor", "Battery level is %d", battery_level);

  if (this->battery_level_ != nullptr)
    this->battery_level_->publish_state(battery_level);

  // Connect Moisture sensor to the Pin via on PCB switch
  digitalWrite(PIN_SWITCH, HIGH);
  nonBlockingDelay(150);

  int moisture_raw = readSensor();

  ESP_LOGD(TAG, "Moisture ADC reading is %d", moisture_raw);

  if (this->moisture_raw_ != nullptr)
    this->moisture_raw_->publish_state(moisture_raw);

  // Battery Drop Correction to normalize to reading at 3.0V
  int moisture_temp = int((float)vcc_reading_3V * (float)moisture_raw / (float)battery_level_raw);
  ESP_LOGI("soilmoisturesensor", "Battery adjusted moisture reading is %d", moisture_temp);

  // Map the moisture to the min and max reading of the sensor
  int moisture = map(moisture_temp, this->adc_dry_at_3v_, this->adc_wet_at_3v_, 0, 100); // Convert to 0 - 100%, 0=Dry, 100=Wet
  if (this->moisture_ != nullptr)
    this->moisture_->publish_state(moisture);
}

#ifdef USE_ESP8266
std::string SoilMoistureSensor::unique_id() { return get_mac_address() + "-soilmoisture"; }
#endif

}  // namespace ap_soil_moisture_sensor
}  // namespace esphome
