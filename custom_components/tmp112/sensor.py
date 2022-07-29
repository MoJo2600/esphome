import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    ICON_EMPTY,
    UNIT_EMPTY,
    CONF_TEMPERATURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS
)

DEPENDENCIES = ['i2c']

CONF_I2C_ADDR = 0x48

tmp112_ns = cg.esphome_ns.namespace('tmp112')
Tmp112 = tmp112_ns.class_('Tmp112', sensor.Sensor, cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = sensor.sensor_schema(
        Tmp112,
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(cv.polling_component_schema('60s')).extend(i2c.i2c_device_schema(CONF_I2C_ADDR))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await sensor.register_sensor(var, config)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    