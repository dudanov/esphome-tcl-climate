import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.cpp_helpers import register_parented

AUTO_LOAD = ["climate", "switch"]
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
