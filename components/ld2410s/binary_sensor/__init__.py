import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, DEVICE_CLASS_OCCUPANCY, DEVICE_CLASS_EMPTY, CONF_HAS_TARGET
from .. import ld2410s_ns, LD2410S, CONF_LD2410S_ID

LD2410SBinarySensor = ld2410s_ns.class_(
    "LD2410SBinarySensor", binary_sensor.BinarySensor, cg.Component
)

CONF_THRESHOLD_UPDATE = "has_threshold_update"

CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(LD2410SBinarySensor),
            cv.GenerateID(CONF_LD2410S_ID): cv.use_id(LD2410S),
            cv.Optional(CONF_HAS_TARGET): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_OCCUPANCY
            ),
            cv.Optional(CONF_THRESHOLD_UPDATE): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_EMPTY
            ),
        }
    ),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if CONF_HAS_TARGET in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_HAS_TARGET])
        cg.add(var.set_presence_sensor(sens))
    if CONF_THRESHOLD_UPDATE in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_THRESHOLD_UPDATE])
        cg.add(var.set_threshold_update_sensor(sens))
    ld2410s = await cg.get_variable(config[CONF_LD2410S_ID])
    cg.add(ld2410s.register_listener(var))
