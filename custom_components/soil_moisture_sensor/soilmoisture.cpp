#include "soilmoisture.h"
#include "esphome/core/log.h"
#include "Arduino.h"
#include "Wire.h"

namespace esphome {
namespace soil_moisture_sensor {

static const char *const TAG = "soilmoisture";

const int TMP_ADDR = 0x48;
const int PIN_SWITCH = D8;

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

void SoilMoistureSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SoilMoistureSensor '%s'...", this->get_name().c_str());
  pinMode(PIN_SWITCH, OUTPUT);
  digitalWrite(PIN_SWITCH, LOW);
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


float SoilMoistureSensor::readTemperature() {
  float temp;

  Wire.beginTransmission(TMP_ADDR);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom(TMP_ADDR, 2);

  while (Wire.available()) {
    int msb = Wire.read();
    int lsb = Wire.read();

    int rawtmp = msb << 8 | lsb;
    int value = rawtmp >> 4;
    temp = value * 0.0625;

    return temp;
  }

  return 0;
}

void SoilMoistureSensor::dump_config() {
  LOG_SENSOR("", "ADC Sensor", this);
  ESP_LOGCONFIG(TAG, "Soil Moisture Sensor");
  // LOG_SENSOR("  ", "Temperature", this->temperature_);
  // LOG_SENSOR("  ", "Moisture", this->moisture_);
  // LOG_SENSOR("  ", "Battery Level", this->battery_level_);
  LOG_UPDATE_INTERVAL(this);
}

float SoilMoistureSensor::get_setup_priority() const { return setup_priority::DATA; }

void SoilMoistureSensor::update() {
  int vccReading3V = 958; 
  int vccReadingRange = 166;

//     int moistureDryReading3V = id(sensor_dry_adc).state;
  ESP_LOGD("soilmoisturesensor", "sensor_dry_adc value is %d", this->adc_dry_at_3v_);
//     int moistureWetReading3V = id(sensor_wet_adc).state;
//     ESP_LOGD("soilmoisturesensor", "sensor_wet_adc state is %d", moistureWetReading3V);

  // Read the temperature
  float temperature = readTemperature();
  ESP_LOGI("soilmoisturesensor", "Temperature is %f", temperature);

  if (this->temperature_ != nullptr)
    this->temperature_->publish_state(temperature);

  // Read battery level
  digitalWrite(PIN_SWITCH, LOW);
  nonBlockingDelay(150);

  int battery_level_raw = this->readSensor();
  ESP_LOGD(TAG, "Battery adc reading is %d", battery_level_raw);

  if (this->battery_level_ != nullptr)
    this->battery_level_raw_->publish_state(battery_level_raw);

  int batteryCharge = map(battery_level_raw, vccReading3V-vccReadingRange, vccReading3V, 0, 100);  
  if (batteryCharge > 100) batteryCharge = 100;
  if (batteryCharge < 0) batteryCharge = 0;

  ESP_LOGI("soilmoisturesensor", "Adjusted battery reading is %d", batteryCharge);

  if (this->battery_level_ != nullptr)
    this->battery_level_->publish_state(batteryCharge);

//     // Connect Moisture sensor to the Pin via on PCB switch
//     digitalWrite(PIN_SWITCH, HIGH);
//     nonBlockingDelay(150);

//     int moisture = 0;
//     int moisture_raw = readSensor();

//     moisture_sensor_adc->publish_state(moisture_raw);
//     ESP_LOGD("soilmoisturesensor", "Moisture adc reading is %d", moisture_raw);

//     // Battery Drop Correction to normalize to reading at 3.0V
//     int moisture_temp = int((float)vccReading3V * (float)moisture_raw / (float)batteryCharge_raw);
//     ESP_LOGI("soilmoisturesensor", "Battery adjusted moisture reading is %d", moisture_temp);

//     // Map the moisture to the min and max reading of the sensor
//     moisture = map(moisture_temp, moistureDryReading3V, moistureWetReading3V, 0, 100); // Convert to 0 - 100%, 0=Dry, 100=Wet
//     moisture_sensor->publish_state(moisture);

//     float temperature = readTemp();
//     ESP_LOGI("soilmoisturesensor", "Temperature is %f", temperature);
//     temperature_sensor->publish_state(temperature);
  // float value_v = this->sample();
  // ESP_LOGV(TAG, "'%s': Got voltage=%.4fV", this->get_name().c_str(), value_v);
  // this->publish_state(value_v);
}

#ifdef USE_ESP8266
std::string SoilMoistureSensor::unique_id() { return get_mac_address() + "-soilmoisture"; }
#endif

// class SoilMoistureSensor : public PollingComponent {
//   private:
//   /*
//   * Function: nonBlockingDelay
//   * --------------------------
//   * A delay function that will not block the cpu like delay() does.
//   */
//   void nonBlockingDelay(int waitmillis) {
//     int time_now = millis();
//     while(millis() < time_now + waitmillis) {
//         //wait without stopping the cpu
//         yield();
//     }
//   }

//   /*
//   * Function: readSensor
//   * --------------------
//   * Reads the sensor connected to pin A0
//   * Since the moisture sensor and the battery voltage are
//   * connected to the same input pin, this function can be used
//   * for both tasks.
//   */
//   int readSensor() {
//     int total = 0;
//     int adcVal = 0;
//     int ret = 0;
//     int sampleCount = 3;

//     for(int i = 0; i < sampleCount; i++){
//       adcVal = analogRead(A0);
//       total += adcVal;
//       nonBlockingDelay(50);
//     }

//     ret = int((float)total / (float)sampleCount);

//     return ret;
//   }

//   float readTemp() {
//     float temp;

//     Wire.beginTransmission(TMP_ADDR);
//     Wire.write(0x00);
//     Wire.endTransmission();

//     Wire.requestFrom(TMP_ADDR, 2);

//     while (Wire.available()) {
//       int msb = Wire.read();
//       int lsb = Wire.read();

//       int rawtmp = msb << 8 | lsb;
//       int value = rawtmp >> 4;
//       temp = value * 0.0625;

//       return temp;
//     }

//     return 0;
//   }

//   public:
//   Sensor *battery_sensor = new Sensor();
//   Sensor *battery_sensor_adc = new Sensor();
//   Sensor *moisture_sensor = new Sensor();
//   Sensor *moisture_sensor_adc = new Sensor();
//   Sensor *temperature_sensor = new Sensor();

//   SoilMoistureSensor() : PollingComponent(60000) { }

//   void setup() override {
//     pinMode(PIN_SWITCH, OUTPUT);
//     digitalWrite(PIN_SWITCH, LOW);
//   }

//   void update() override {
//     int vccReading3V = 958; 
//     int vccReadingRange = 166;

//     int moistureDryReading3V = id(sensor_dry_adc).state;
//     ESP_LOGD("soilmoisturesensor", "sensor_dry_adc state is %d", moistureDryReading3V);
//     int moistureWetReading3V = id(sensor_wet_adc).state;
//     ESP_LOGD("soilmoisturesensor", "sensor_wet_adc state is %d", moistureWetReading3V);

//     // Read battery level
//     digitalWrite(PIN_SWITCH, LOW);
//     nonBlockingDelay(150);

//     int batteryCharge_raw = readSensor();
//     ESP_LOGD("soilmoisturesensor", "Battery adc reading is %d", batteryCharge_raw);
//     battery_sensor_adc->publish_state(batteryCharge_raw);

//     int batteryCharge = map(batteryCharge_raw, vccReading3V-vccReadingRange, vccReading3V, 0, 100);  
//     if (batteryCharge > 100) batteryCharge = 100;
//     if (batteryCharge < 0) batteryCharge = 0;

//     ESP_LOGI("soilmoisturesensor", "Adjusted battery reading is %d", batteryCharge);

//     battery_sensor->publish_state(batteryCharge);

//     // Connect Moisture sensor to the Pin via on PCB switch
//     digitalWrite(PIN_SWITCH, HIGH);
//     nonBlockingDelay(150);

//     int moisture = 0;
//     int moisture_raw = readSensor();

//     moisture_sensor_adc->publish_state(moisture_raw);
//     ESP_LOGD("soilmoisturesensor", "Moisture adc reading is %d", moisture_raw);

//     // Battery Drop Correction to normalize to reading at 3.0V
//     int moisture_temp = int((float)vccReading3V * (float)moisture_raw / (float)batteryCharge_raw);
//     ESP_LOGI("soilmoisturesensor", "Battery adjusted moisture reading is %d", moisture_temp);

//     // Map the moisture to the min and max reading of the sensor
//     moisture = map(moisture_temp, moistureDryReading3V, moistureWetReading3V, 0, 100); // Convert to 0 - 100%, 0=Dry, 100=Wet
//     moisture_sensor->publish_state(moisture);

//     float temperature = readTemp();
//     ESP_LOGI("soilmoisturesensor", "Temperature is %f", temperature);
//     temperature_sensor->publish_state(temperature);
//   }
// };

}  // namespace soil_moisture_sensor
}  // namespace esphome
