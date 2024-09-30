import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_FREQUENCY,
    UNIT_SECOND,
    UNIT_HERTZ,
    ENTITY_CATEGORY_CONFIG,
    ICON_MOTION_SENSOR,
    ICON_TIMELAPSE,
    ICON_PULSE
)
from .. import CONF_LD2410S_ID, LD2410S, ld2410s_ns

LD2410SMaxDistanceNumber = ld2410s_ns.class_("LD2410SMaxDistanceNumber", number.Number)
LD2410SMinDistanceNumber = ld2410s_ns.class_("LD2410SMinDistanceNumber", number.Number)
LD2410SDelayNumber = ld2410s_ns.class_("LD2410SDelayNumber", number.Number)
LD2410SStatusReportingFreqNumber = ld2410s_ns.class_("LD2410SStatusReportingFreqNumber", number.Number)
LD2410SDistReportingFreqNumber = ld2410s_ns.class_("LD2410SDistReportingFreqNumber", number.Number)

CONF_MAX_DISTANCE = "max_distance"
CONF_MIN_DISTANCE = "min_distance"
CONF_NO_DELAY = "no_delay"
DISTANCE_GROUP = "distance_group"
CONF_STATUS_REPORT_FREQ = "status_reporting_frequency"
CONF_DISTANCE_REPORT_FREQ = "distance_reporting_frequency"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LD2410S_ID): cv.use_id(LD2410S),
        cv.Inclusive(CONF_MAX_DISTANCE, DISTANCE_GROUP) : number.number_schema(
            LD2410SMaxDistanceNumber,
            device_class=DEVICE_CLASS_DISTANCE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_MOTION_SENSOR,
        ),
        cv.Inclusive(CONF_MIN_DISTANCE, DISTANCE_GROUP) : number.number_schema(
            LD2410SMinDistanceNumber,
            device_class=DEVICE_CLASS_DISTANCE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_MOTION_SENSOR,
        ),
        cv.Optional(CONF_NO_DELAY) : number.number_schema(
            LD2410SDelayNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_TIMELAPSE,
        ),
        cv.Optional(CONF_STATUS_REPORT_FREQ) : number.number_schema(
            LD2410SStatusReportingFreqNumber,
            device_class=DEVICE_CLASS_FREQUENCY,
            unit_of_measurement=UNIT_HERTZ,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_PULSE,
        ),
        cv.Optional(CONF_DISTANCE_REPORT_FREQ) : number.number_schema(
            LD2410SDistReportingFreqNumber,
            device_class=DEVICE_CLASS_FREQUENCY,
            unit_of_measurement=UNIT_HERTZ,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_PULSE,
        ),
    }
)

async def to_code(config):
    LD2410S_component = await cg.get_variable(config[CONF_LD2410S_ID])
    if max_distance_config := config.get(CONF_MAX_DISTANCE):
        n = await number.new_number(max_distance_config, min_value=0, max_value=8.4, step=1)
        await cg.register_parented(n, config[CONF_LD2410S_ID])
        cg.add(LD2410S_component.set_max_distance_number(n))
    if min_distance_config := config.get(CONF_MIN_DISTANCE):
        n = await number.new_number(min_distance_config, min_value=0, max_value=8.4, step=1)
        await cg.register_parented(n, config[CONF_LD2410S_ID])
        cg.add(LD2410S_component.set_min_distance_number(n))
    if no_delay_config := config.get(CONF_NO_DELAY):
        n = await number.new_number(no_delay_config, min_value=10, max_value=120, step=1)
        await cg.register_parented(n, config[CONF_LD2410S_ID])
        cg.add(LD2410S_component.set_no_delay_number(n))
    if status_reporting_freq_config := config.get(CONF_STATUS_REPORT_FREQ):
        n = await number.new_number(status_reporting_freq_config, min_value=0.5, max_value=8, step=0.5)
        await cg.register_parented(n, config[CONF_LD2410S_ID])
        cg.add(LD2410S_component.set_status_reporting_freq_number(n))
    if distance_reporting_freq_config := config.get(CONF_DISTANCE_REPORT_FREQ):
        n = await number.new_number(distance_reporting_freq_config, min_value=0.5, max_value=8, step=0.5)
        await cg.register_parented(n, config[CONF_LD2410S_ID])
        cg.add(LD2410S_component.set_distance_reporting_freq_number(n))