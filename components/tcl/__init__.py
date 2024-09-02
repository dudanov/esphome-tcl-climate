import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.cpp_helpers import register_parented

DOMAIN = "tcl"
CONF_TCL_ID = "tcl_id"
CONF_FORCE = "force"


tcl_ns = cg.esphome_ns.namespace(DOMAIN)
TclBase = tcl_ns.class_("TclBase", uart.UARTDevice, cg.PollingComponent)


def register_tcl(var, config):
    return register_parented(var, config[CONF_TCL_ID])


def tcl_parented_schema(class_=TclBase):
    return cv.Schema(
        {
            cv.GenerateID(CONF_TCL_ID): cv.use_id(class_),
        }
    )


ForceOnAction = tcl_ns.class_("ForceOnAction", automation.Action)
ForceOffAction = tcl_ns.class_("ForceOffAction", automation.Action)
BeeperOnAction = tcl_ns.class_("BeeperOnAction", automation.Action)
BeeperOffAction = tcl_ns.class_("BeeperOffAction", automation.Action)
DisplayOnAction = tcl_ns.class_("DisplayOnAction", automation.Action)
DisplayOffAction = tcl_ns.class_("DisplayOffAction", automation.Action)

TCL_SIMPLE_ACTION_SCHEMA = automation.maybe_conf(CONF_TCL_ID, tcl_parented_schema())


TCL_BASE_SCHEMA = uart.UART_DEVICE_SCHEMA.extend(cv.polling_component_schema("5s"))


# Регистрация событий включения и отключения пищалки кондиционера
@automation.register_action("tcl.beeper_on", BeeperOnAction, TCL_SIMPLE_ACTION_SCHEMA)
@automation.register_action("tcl.beeper_off", BeeperOffAction, TCL_SIMPLE_ACTION_SCHEMA)
# Регистрация событий включения и отключения дисплея кондиционера
@automation.register_action("tcl.display_on", DisplayOnAction, TCL_SIMPLE_ACTION_SCHEMA)
@automation.register_action(
    "tcl.display_off", DisplayOffAction, TCL_SIMPLE_ACTION_SCHEMA
)
# Регистрация событий включения и отключения принудительного применения настроек
@automation.register_action("tcl.force_on", ForceOnAction, TCL_SIMPLE_ACTION_SCHEMA)
@automation.register_action("tcl.force_off", ForceOffAction, TCL_SIMPLE_ACTION_SCHEMA)
async def base_actions_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_TCL_ID])
    return var


TCL_FV_SCHEMA = uart.final_validate_device_schema(
    name=DOMAIN,
    baud_rate=9600,
    require_rx=True,
    require_tx=True,
    data_bits=8,
    parity="EVEN",
    stop_bits=1,
)
