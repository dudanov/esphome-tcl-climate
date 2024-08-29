import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.switch import Switch, new_switch, switch_schema
from esphome.const import CONF_BEEPER, CONF_DISPLAY, ENTITY_CATEGORY_CONFIG, CONF_NAME
from esphome import automation

from .. import CONF_TCL_ID, TclBase, register_tcl, tcl_ns, CONF_FORCE


BaseSwitch = tcl_ns.class_("BaseSwitch", Switch, cg.Parented)
BeeperSwitch = tcl_ns.class_("BeeperSwitch", BaseSwitch)
DisplaySwitch = tcl_ns.class_("DisplaySwitch", BaseSwitch)
ForceSwitch = tcl_ns.class_("ForceSwitch", BaseSwitch)


def tcl_simple(_class):
    return automation.maybe_conf(
        CONF_NAME,
        switch_schema(
            _class,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:volume-source",
            default_restore_mode="RESTORE_DEFAULT_ON",
        ),
    )


SWITCH_TYPES = {
    CONF_BEEPER: tcl_simple(BeeperSwitch),
    CONF_DISPLAY: tcl_simple(DisplaySwitch),
    CONF_FORCE: tcl_simple(ForceSwitch),
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TCL_ID): cv.use_id(TclBase),
    }
).extend({cv.Optional(k): v for k, v in SWITCH_TYPES.items()})


async def to_code(config):
    for option in SWITCH_TYPES:
        if conf := config.get(option):
            var = await new_switch(conf)
            await register_tcl(var, config)
