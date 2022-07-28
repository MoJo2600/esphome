import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_BATTERY_LEVEL,
    CONF_BATTERY_VOLTAGE,
    CONF_MAC_ADDRESS,
    CONF_HUMIDITY,
    CONF_SIGNAL_STRENGTH,
    CONF_TEMPERATURE,
    CONF_ID,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ENTITY_CATEGORY_DIAGNOSTIC,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_DECIBEL_MILLIWATT,
    UNIT_PERCENT,
    UNIT_VOLT,
    UNIT_EMPTY,
)

CODEOWNERS = ["@mojo2600"]

soil_moisture_sensor_ns = cg.esphome_ns.namespace("soil_moisture_sensor")
SoilMoistureSensor = soil_moisture_sensor_ns.class_(
    "SoilMoistureSensor", sensor.Sensor, cg.PollingComponent
)

CONF_ADC_DRY_AT_3V="adc_dry_at_3v"
CONF_BATTERY_LEVEL_RAW="battery_level_raw"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SoilMoistureSensor),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_LEVEL): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_BATTERY,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_BATTERY_LEVEL_RAW): sensor.sensor_schema(
                unit_of_measurement=UNIT_EMPTY,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_BATTERY,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_ADC_DRY_AT_3V, default=248): cv.int_range(
                min=0,
                max=1024
            ),
        }
    ).extend(cv.polling_component_schema("60s"))
)

# CONFIG_SCHEMA = (
#     cv.Schema(
#         {
#             cv.GenerateID(): cv.declare_id(SoilMoistureSensor),
#             cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
#                 unit_of_measurement=UNIT_CELSIUS,
#                 accuracy_decimals=1,
#                 device_class=DEVICE_CLASS_TEMPERATURE,
#                 state_class=STATE_CLASS_MEASUREMENT,
#             ),
#             cv.Optional(CONF_MOISTURE): sensor.sensor_schema(
#                 unit_of_measurement=UNIT_PERCENT,
#                 accuracy_decimals=0,
#                 device_class=DEVICE_CLASS_HUMIDITY,
#                 state_class=STATE_CLASS_MEASUREMENT,
#             ),
#             cv.Optional(CONF_BATTERY_LEVEL): sensor.sensor_schema(
#                 unit_of_measurement=UNIT_PERCENT,
#                 accuracy_decimals=0,
#                 device_class=DEVICE_CLASS_BATTERY,
#                 state_class=STATE_CLASS_MEASUREMENT,
#                 entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
#             )
#         }
#     )
#     .extend(cv.polling_component_schema("60s"))
# )


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_temperature(sens))
    # if CONF_MOISTURE in config:
    #     sens = await sensor.new_sensor(config[CONF_MOISTURE])
    #     cg.add(var.set_humidity(sens))
    if CONF_BATTERY_LEVEL in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_LEVEL])
        cg.add(var.set_battery_level(sens))
    if CONF_BATTERY_LEVEL_RAW in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_LEVEL_RAW])
        cg.add(var.set_battery_level_raw(sens))
    cg.add(var.set_adc_dry_at_3v(config[CONF_ADC_DRY_AT_3V]))