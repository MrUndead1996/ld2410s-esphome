import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
    ENTITY_CATEGORY_CONFIG,
    ICON_BUG,
    DEVICE_CLASS_IDENTIFY,
    DEVICE_CLASS_RESTART,
    DEVICE_CLASS_UPDATE,
    ICON_RESTART_ALERT,
    ICON_MEMORY
)
from .. import CONF_LD2410S_ID, LD2410S, ld2410s_ns

LD2410SEnableConfigButton = ld2410s_ns.class_("LD2410SEnableConfigButton", button.Button)
LD2410SDisableConfigButton = ld2410s_ns.class_("LD2410SDisableConfigButton", button.Button)
LD2410SApplyConfigButton = ld2410s_ns.class_("LD2410SApplyConfigButton", button.Button)
LD2410SAutoConfigThreshold = ld2410s_ns.class_("LD2410SAutoConfigThreshold", button.Button)

CONF_ENABLE_CONFIG = "enable_config"
CONF_DISABLE_CONFIG = "disable_config"
CONF_APPLY_CONFIG = "apply_config"
CONF_AUTO_THRESHOLD = "auto_threshold"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_LD2410S_ID): cv.use_id(LD2410S),
    cv.Optional(CONF_ENABLE_CONFIG): button.button_schema(
        LD2410SEnableConfigButton,
        device_class=DEVICE_CLASS_IDENTIFY,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon=ICON_BUG
    ),
    cv.Optional(CONF_DISABLE_CONFIG): button.button_schema(
        LD2410SDisableConfigButton,
        device_class=DEVICE_CLASS_IDENTIFY,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon=ICON_BUG
    ),
    cv.Required(CONF_APPLY_CONFIG): button.button_schema(
        LD2410SApplyConfigButton,
        device_class=DEVICE_CLASS_RESTART,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon=ICON_RESTART_ALERT,
    ),
    cv.Required(CONF_AUTO_THRESHOLD): button.button_schema(
        LD2410SAutoConfigThreshold,
        device_class=DEVICE_CLASS_UPDATE,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:icon-refresh",
    )
}

async def to_code(config):
    ld2410s_component = await cg.get_variable(config[CONF_LD2410S_ID])
    if enable_confing := config.get(CONF_ENABLE_CONFIG):
        b = await button.new_button(enable_confing)
        await cg.register_parented(b, config[CONF_LD2410S_ID])
        cg.add(ld2410s_component.set_enable_config_button(b))
    if disable_confing := config.get(CONF_DISABLE_CONFIG):
        b = await button.new_button(disable_confing)
        await cg.register_parented(b, config[CONF_LD2410S_ID])
        cg.add(ld2410s_component.set_disable_config_button(b))
    if apply_config := config.get(CONF_APPLY_CONFIG):
        b = await button.new_button(apply_config)
        await cg.register_parented(b, config[CONF_LD2410S_ID])
        cg.add(ld2410s_component.set_apply_config_button(b))
    if auto_threshold := config.get(CONF_AUTO_THRESHOLD):
        b = await button.new_button(auto_threshold)
        await cg.register_parented(b, config[CONF_LD2410S_ID])
        cg.add(ld2410s_component.set_auto_threshold_button(b))
    