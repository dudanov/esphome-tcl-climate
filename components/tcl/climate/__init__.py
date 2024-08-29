import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import climate, uart
from esphome.components.climate import (
    CONF_CURRENT_TEMPERATURE,
    ClimateMode,
    ClimatePreset,
    ClimateSwingMode,
)
from esphome.const import (
    CONF_BEEPER,
    CONF_ID,
    CONF_MAX_TEMPERATURE,
    CONF_MIN_TEMPERATURE,
    CONF_SUPPORTED_FAN_MODES,
    CONF_SUPPORTED_MODES,
    CONF_SUPPORTED_PRESETS,
    CONF_SUPPORTED_SWING_MODES,
    CONF_TARGET_TEMPERATURE,
    CONF_TEMPERATURE_STEP,
    CONF_VISUAL,
    ENTITY_CATEGORY_CONFIG,
)
from esphome.components.switch import (
    Switch,
    new_switch,
    switch_schema,
)
from esphome.cpp_helpers import register_parented

NAME = "tcl"
AUTO_LOAD = ["climate", "switch"]
CODEOWNERS = ["@I-am-nightingale", "@xaxexa", "@junkfix"]
DEPENDENCIES = ["climate", "uart"]

TCLAC_MIN_TEMPERATURE = 16.0
TCLAC_MAX_TEMPERATURE = 31.0
TCLAC_TARGET_TEMPERATURE_STEP = 1.0
TCLAC_CURRENT_TEMPERATURE_STEP = 1.0

CONF_TCL_ID = "tcl_id"
CONF_DISPLAY = "display"
CONF_FORCE_MODE = "force_mode"
CONF_VERTICAL_AIRFLOW = "vertical_airflow"
CONF_HORIZONTAL_AIRFLOW = "horizontal_airflow"
CONF_VERTICAL_SWING_MODE = "vertical_swing_mode"
CONF_HORIZONTAL_SWING_MODE = "horizontal_swing_mode"


def register_tcl(var, config):
    return register_parented(var, config[CONF_TCL_ID])


def tcl_parented_schema(class_):
    return cv.Schema(
        {
            cv.GenerateID(CONF_TCL_ID): cv.use_id(class_),
        }
    )


tcl_ns = cg.esphome_ns.namespace(NAME)
TclClimate = tcl_ns.class_(
    "TclClimate", uart.UARTDevice, climate.Climate, cg.PollingComponent
)


BeeperSwitch = tcl_ns.class_("BeeperSwitch", Switch)
BEEPER_SWITCH_SCHEMA = switch_schema(
    BeeperSwitch,
    entity_category=ENTITY_CATEGORY_CONFIG,
    icon="mdi:volume-source",
    default_restore_mode="RESTORE_DEFAULT_ON",
).extend(tcl_parented_schema(TclClimate))

DisplaySwitch = tcl_ns.class_("DisplaySwitch", Switch)
DISPLAY_SWITCH_SCHEMA = switch_schema(
    DisplaySwitch,
    entity_category=ENTITY_CATEGORY_CONFIG,
    icon="mdi:volume-source",
    default_restore_mode="RESTORE_DEFAULT_ON",
).extend(tcl_parented_schema(TclClimate))


async def new_tcl_switch(config):
    var = await new_switch(config)
    await register_tcl(var, config)


SUPPORTED_FAN_MODES_OPTIONS = {
    "AUTO": ClimateMode.CLIMATE_FAN_AUTO,  # Доступен всегда
    "QUIET": ClimateMode.CLIMATE_FAN_QUIET,
    "LOW": ClimateMode.CLIMATE_FAN_LOW,
    "MIDDLE": ClimateMode.CLIMATE_FAN_MIDDLE,
    "MEDIUM": ClimateMode.CLIMATE_FAN_MEDIUM,
    "HIGH": ClimateMode.CLIMATE_FAN_HIGH,
    "FOCUS": ClimateMode.CLIMATE_FAN_FOCUS,
    "DIFFUSE": ClimateMode.CLIMATE_FAN_DIFFUSE,
}

SUPPORTED_SWING_MODES_OPTIONS = {
    "OFF": ClimateSwingMode.CLIMATE_SWING_OFF,  # Доступен всегда
    "VERTICAL": ClimateSwingMode.CLIMATE_SWING_VERTICAL,
    "HORIZONTAL": ClimateSwingMode.CLIMATE_SWING_HORIZONTAL,
    "BOTH": ClimateSwingMode.CLIMATE_SWING_BOTH,
}

SUPPORTED_CLIMATE_MODES_OPTIONS = {
    "OFF": ClimateMode.CLIMATE_MODE_OFF,  # Доступен всегда
    "AUTO": ClimateMode.CLIMATE_MODE_AUTO,  # Доступен всегда
    "COOL": ClimateMode.CLIMATE_MODE_COOL,
    "HEAT": ClimateMode.CLIMATE_MODE_HEAT,
    "DRY": ClimateMode.CLIMATE_MODE_DRY,
    "FAN_ONLY": ClimateMode.CLIMATE_MODE_FAN_ONLY,
}

SUPPORTED_CLIMATE_PRESETS_OPTIONS = {
    "NONE": ClimatePreset.CLIMATE_PRESET_NONE,  # Доступен всегда
    "ECO": ClimatePreset.CLIMATE_PRESET_ECO,
    "SLEEP": ClimatePreset.CLIMATE_PRESET_SLEEP,
    "COMFORT": ClimatePreset.CLIMATE_PRESET_COMFORT,
}

VerticalSwingDirection = tcl_ns.enum("VerticalSwingDirection", True)
VERTICAL_SWING_DIRECTION_OPTIONS = {
    "UP_DOWN": VerticalSwingDirection.UPDOWN,
    "UPSIDE": VerticalSwingDirection.UPSIDE,
    "DOWNSIDE": VerticalSwingDirection.DOWNSIDE,
}

HorizontalSwingDirection = tcl_ns.enum("HorizontalSwingDirection", True)
HORIZONTAL_SWING_DIRECTION_OPTIONS = {
    "LEFT_RIGHT": HorizontalSwingDirection.LEFT_RIGHT,
    "LEFTSIDE": HorizontalSwingDirection.LEFTSIDE,
    "CENTER": HorizontalSwingDirection.CENTER,
    "RIGHTSIDE": HorizontalSwingDirection.RIGHTSIDE,
}

AirflowVerticalDirection = tcl_ns.enum("AirflowVerticalDirection")
AIRFLOW_VERTICAL_DIRECTION_OPTIONS = {
    "LAST": AirflowVerticalDirection.LAST,
    "MAX_UP": AirflowVerticalDirection.MAX_UP,
    "UP": AirflowVerticalDirection.UP,
    "CENTER": AirflowVerticalDirection.CENTER,
    "DOWN": AirflowVerticalDirection.DOWN,
    "MAX_DOWN": AirflowVerticalDirection.MAX_DOWN,
}

AirflowHorizontalDirection = tcl_ns.enum("AirflowHorizontalDirection", True)
AIRFLOW_HORIZONTAL_DIRECTION_OPTIONS = {
    "LAST": AirflowHorizontalDirection.LAST,
    "MAX_LEFT": AirflowHorizontalDirection.MAX_LEFT,
    "LEFT": AirflowHorizontalDirection.LEFT,
    "CENTER": AirflowHorizontalDirection.CENTER,
    "RIGHT": AirflowHorizontalDirection.RIGHT,
    "MAX_RIGHT": AirflowHorizontalDirection.MAX_RIGHT,
}


# Проверка конфигурации интерфейса и принятие значений по умолчанию
def validate_visual(config):
    if CONF_VISUAL in config:
        visual_config = config[CONF_VISUAL]
        if CONF_MIN_TEMPERATURE in visual_config:
            min_temp = visual_config[CONF_MIN_TEMPERATURE]
            if min_temp < TCLAC_MIN_TEMPERATURE:
                raise cv.Invalid(
                    f"Указанная интерфейсная минимальная температура в {min_temp} ниже допустимой {TCLAC_MIN_TEMPERATURE} для кондиционера"
                )
        else:
            config[CONF_VISUAL][CONF_MIN_TEMPERATURE] = TCLAC_MIN_TEMPERATURE
        if CONF_MAX_TEMPERATURE in visual_config:
            max_temp = visual_config[CONF_MAX_TEMPERATURE]
            if max_temp > TCLAC_MAX_TEMPERATURE:
                raise cv.Invalid(
                    f"Указанная интерфейсная максимальная температура в {max_temp} выше допустимой {TCLAC_MAX_TEMPERATURE} для кондиционера"
                )
        else:
            config[CONF_VISUAL][CONF_MAX_TEMPERATURE] = TCLAC_MAX_TEMPERATURE
        if CONF_TEMPERATURE_STEP in visual_config:
            temp_step = config[CONF_VISUAL][CONF_TEMPERATURE_STEP][
                CONF_TARGET_TEMPERATURE
            ]
            if ((int)(temp_step * 2)) / 2 != temp_step:
                raise cv.Invalid(
                    f"Указанный шаг температуры {temp_step} не корректен, должен быть кратен 1"
                )
        else:
            config[CONF_VISUAL][CONF_TEMPERATURE_STEP] = {
                CONF_TARGET_TEMPERATURE: TCLAC_TARGET_TEMPERATURE_STEP,
                CONF_CURRENT_TEMPERATURE: TCLAC_CURRENT_TEMPERATURE_STEP,
            }
    else:
        config[CONF_VISUAL] = {
            CONF_MIN_TEMPERATURE: TCLAC_MIN_TEMPERATURE,
            CONF_MAX_TEMPERATURE: TCLAC_MAX_TEMPERATURE,
            CONF_TEMPERATURE_STEP: {
                CONF_TARGET_TEMPERATURE: TCLAC_TARGET_TEMPERATURE_STEP,
                CONF_CURRENT_TEMPERATURE: TCLAC_CURRENT_TEMPERATURE_STEP,
            },
        }
    return config


# Проверка конфигурации компонента и принятие значений по умолчанию
CONFIG_SCHEMA = cv.All(
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(TclClimate),
            cv.Optional(CONF_BEEPER): BEEPER_SWITCH_SCHEMA,
            cv.Optional(CONF_DISPLAY): DISPLAY_SWITCH_SCHEMA,
            cv.Optional(CONF_FORCE_MODE, default=True): cv.boolean,
            cv.Optional(CONF_VERTICAL_AIRFLOW, default="CENTER"): cv.ensure_list(
                cv.enum(AIRFLOW_VERTICAL_DIRECTION_OPTIONS, upper=True)
            ),
            cv.Optional(CONF_VERTICAL_SWING_MODE, default="UP_DOWN"): cv.ensure_list(
                cv.enum(VERTICAL_SWING_DIRECTION_OPTIONS, upper=True)
            ),
            cv.Optional(CONF_HORIZONTAL_AIRFLOW, default="CENTER"): cv.ensure_list(
                cv.enum(AIRFLOW_HORIZONTAL_DIRECTION_OPTIONS, upper=True)
            ),
            cv.Optional(
                CONF_HORIZONTAL_SWING_MODE, default="LEFT_RIGHT"
            ): cv.ensure_list(cv.enum(HORIZONTAL_SWING_DIRECTION_OPTIONS, upper=True)),
            cv.Optional(
                CONF_SUPPORTED_PRESETS,
                default=[
                    "NONE",
                    "ECO",
                    "SLEEP",
                    "COMFORT",
                ],
            ): cv.ensure_list(cv.enum(SUPPORTED_CLIMATE_PRESETS_OPTIONS, upper=True)),
            cv.Optional(
                CONF_SUPPORTED_SWING_MODES,
                default=[
                    "OFF",
                    "VERTICAL",
                    "HORIZONTAL",
                    "BOTH",
                ],
            ): cv.ensure_list(cv.enum(SUPPORTED_SWING_MODES_OPTIONS, upper=True)),
            cv.Optional(
                CONF_SUPPORTED_MODES,
                default=[
                    "OFF",
                    "AUTO",
                    "COOL",
                    "HEAT",
                    "DRY",
                    "FAN_ONLY",
                ],
            ): cv.ensure_list(cv.enum(SUPPORTED_CLIMATE_MODES_OPTIONS, upper=True)),
            cv.Optional(
                CONF_SUPPORTED_FAN_MODES,
                default=[
                    "AUTO",
                    "QUIET",
                    "LOW",
                    "MIDDLE",
                    "MEDIUM",
                    "HIGH",
                    "FOCUS",
                    "DIFFUSE",
                ],
            ): cv.ensure_list(cv.enum(SUPPORTED_FAN_MODES_OPTIONS, upper=True)),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA),
    validate_visual,
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    name=NAME,
    baud_rate=9600,
    require_rx=True,
    require_tx=True,
    data_bits=8,
    parity="EVEN",
    stop_bits=1,
)

ForceOnAction = tcl_ns.class_("ForceOnAction", automation.Action)
ForceOffAction = tcl_ns.class_("ForceOffAction", automation.Action)
BeeperOnAction = tcl_ns.class_("BeeperOnAction", automation.Action)
BeeperOffAction = tcl_ns.class_("BeeperOffAction", automation.Action)
DisplayOnAction = tcl_ns.class_("DisplayOnAction", automation.Action)
DisplayOffAction = tcl_ns.class_("DisplayOffAction", automation.Action)
VerticalAirflowAction = tcl_ns.class_("VerticalAirflowAction", automation.Action)
HorizontalAirflowAction = tcl_ns.class_("HorizontalAirflowAction", automation.Action)
VerticalSwingDirectionAction = tcl_ns.class_(
    "VerticalSwingDirectionAction", automation.Action
)
HorizontalSwingDirectionAction = tcl_ns.class_(
    "HorizontalSwingDirectionAction", automation.Action
)

TCL_ACTION_BASE_SCHEMA = automation.maybe_conf(
    CONF_TCL_ID,
    {
        cv.GenerateID(CONF_TCL_ID): cv.use_id(TclClimate),
    },
)


def tcl_templated_schema(conf, validator):
    return automation.maybe_conf(
        conf,
        {
            cv.GenerateID(CONF_TCL_ID): cv.use_id(TclClimate),
            cv.Required(conf): cv.templatable(validator),
        },
    )


# Регистрация событий включения и отключения пищалки кондиционера
@automation.register_action("tcl.beeper_on", BeeperOnAction, TCL_ACTION_BASE_SCHEMA)
@automation.register_action("tcl.beeper_off", BeeperOffAction, TCL_ACTION_BASE_SCHEMA)
# Регистрация событий включения и отключения дисплея кондиционера
@automation.register_action("tcl.display_on", DisplayOnAction, TCL_ACTION_BASE_SCHEMA)
@automation.register_action("tcl.display_off", DisplayOffAction, TCL_ACTION_BASE_SCHEMA)
# Регистрация событий включения и отключения принудительного применения настроек
@automation.register_action("tcl.force_on", ForceOnAction, TCL_ACTION_BASE_SCHEMA)
@automation.register_action("tcl.force_off", ForceOffAction, TCL_ACTION_BASE_SCHEMA)
async def base_actions_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_TCL_ID])
    return var


# Регистрация события установки вертикальной фиксации заслонки
@automation.register_action(
    "tcl.set_vertical_airflow",
    VerticalAirflowAction,
    tcl_templated_schema(
        CONF_VERTICAL_AIRFLOW, cv.enum(AIRFLOW_VERTICAL_DIRECTION_OPTIONS)
    ),
)
async def tclac_set_vertical_airflow_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_TCL_ID])
    template_ = await cg.templatable(
        config[CONF_VERTICAL_AIRFLOW], args, AirflowVerticalDirection
    )
    cg.add(var.set_direction(template_))
    return var


# Регистрация события установки горизонтальной фиксации заслонок
@automation.register_action(
    "tcl.set_horizontal_airflow",
    HorizontalAirflowAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(TclClimate),
            cv.Required(CONF_HORIZONTAL_AIRFLOW): cv.templatable(
                cv.enum(AIRFLOW_HORIZONTAL_DIRECTION_OPTIONS, upper=True)
            ),
        }
    ),
)
async def tclac_set_horizontal_airflow_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(
        config[CONF_HORIZONTAL_AIRFLOW], args, AirflowHorizontalDirection
    )
    cg.add(var.set_direction(template_))
    return var


# Регистрация события установки вертикального качания шторки
@automation.register_action(
    "tcl.set_vertical_swing_direction",
    VerticalSwingDirectionAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(TclClimate),
            cv.Required(CONF_VERTICAL_SWING_MODE): cv.templatable(
                cv.enum(VERTICAL_SWING_DIRECTION_OPTIONS, upper=True)
            ),
        }
    ),
)
async def tclac_set_vertical_swing_direction_to_code(
    config, action_id, template_arg, args
):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(
        config[CONF_VERTICAL_SWING_MODE], args, VerticalSwingDirection
    )
    cg.add(var.set_swing_direction(template_))
    return var


# Регистрация события установки горизонтального качания шторок
@automation.register_action(
    "tcl.set_horizontal_swing_direction",
    HorizontalSwingDirectionAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(TclClimate),
            cv.Required(CONF_HORIZONTAL_SWING_MODE): cv.templatable(
                cv.enum(HORIZONTAL_SWING_DIRECTION_OPTIONS, upper=True)
            ),
        }
    ),
)
async def tclac_set_horizontal_swing_direction_to_code(
    config, action_id, template_arg, args
):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(
        config[CONF_HORIZONTAL_SWING_MODE], args, HorizontalSwingDirection
    )
    cg.add(var.set_swing_direction(template_))
    return var


# Добавление конфигурации в код
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    await climate.register_climate(var, config)

    if CONF_BEEPER in config:
        await new_tcl_switch(config[CONF_BEEPER])
    if CONF_DISPLAY in config:
        await new_tcl_switch(config[CONF_DISPLAY])
    if CONF_FORCE_MODE in config:
        cg.add(var.set_force_mode_state(config[CONF_FORCE_MODE]))
    if CONF_SUPPORTED_MODES in config:
        cg.add(var.set_supported_modes(config[CONF_SUPPORTED_MODES]))
    if CONF_SUPPORTED_PRESETS in config:
        cg.add(var.set_supported_presets(config[CONF_SUPPORTED_PRESETS]))
    if CONF_SUPPORTED_FAN_MODES in config:
        cg.add(var.set_supported_fan_modes(config[CONF_SUPPORTED_FAN_MODES]))
    if CONF_SUPPORTED_SWING_MODES in config:
        cg.add(var.set_supported_swing_modes(config[CONF_SUPPORTED_SWING_MODES]))
