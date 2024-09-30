import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_CONFIG
from .. import CONF_LD2410S_ID, LD2410S, ld2410s_ns

CONF_RESPONSE_SPEED = "response_speed"
CONF_SELECTS = [
    "Normal",
    "Fast"
]

LD2420ResponseSpeedSelect = ld2410s_ns.class_("LD2420ResponseSpeedSelect", cg.Component)

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_LD2410S_ID): cv.use_id(LD2410S),
    cv.Required(CONF_RESPONSE_SPEED): select.select_schema(
        LD2420ResponseSpeedSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ),
}


async def to_code(config):
    LD2410S_component = await cg.get_variable(config[CONF_LD2410S_ID])
    if response_speed_config := config.get(CONF_RESPONSE_SPEED):
        sel = await select.new_select(
            response_speed_config,
            options=CONF_SELECTS,
        )
        await cg.register_parented(sel, config[CONF_LD2410S_ID])
        cg.add(LD2410S_component.set_response_speed_select(sel))
