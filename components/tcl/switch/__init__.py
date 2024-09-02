import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components.switch import Switch, new_switch, switch_schema
from esphome.const import CONF_BEEPER, CONF_DISPLAY, CONF_NAME, ENTITY_CATEGORY_CONFIG

from .. import CONF_FORCE, CONF_TCL_ID, TclBase, tcl_ns

BaseSwitch = tcl_ns.class_("BaseSwitch", Switch, cg.Parented)
BeeperSwitch = tcl_ns.class_("BeeperSwitch", BaseSwitch)
DisplaySwitch = tcl_ns.class_("DisplaySwitch", BaseSwitch)
ForceSwitch = tcl_ns.class_("ForceSwitch", BaseSwitch)


_SCHEMAS = {
    CONF_BEEPER: automation.maybe_conf(
        CONF_NAME,
        switch_schema(
            BeeperSwitch,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:volume-source",
            default_restore_mode="RESTORE_DEFAULT_ON",
        ),
    ),
    CONF_DISPLAY: automation.maybe_conf(
        CONF_NAME,
        switch_schema(
            DisplaySwitch,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:theme-light-dark",
            default_restore_mode="RESTORE_DEFAULT_ON",
        ),
    ),
    CONF_FORCE: automation.maybe_conf(
        CONF_NAME,
        switch_schema(
            ForceSwitch,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:timer-lock-open-outline",
            default_restore_mode="RESTORE_DEFAULT_ON",
        ),
    ),
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TCL_ID): cv.use_id(TclBase),
    }
).extend({cv.Optional(k): v for k, v in _SCHEMAS.items()})


async def to_code(config):
    tcl = await cg.get_variable(config[CONF_TCL_ID])
    for option in _SCHEMAS:
        if conf := config.get(option):
            var = await new_switch(conf)
            await cg.register_parented(var, tcl)
            cg.add(getattr(tcl, f"set_{option}_switch")(var))
