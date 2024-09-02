from esphome.core import coroutine
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
    CONF_DISPLAY,
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
)

from .. import (
    CONF_FORCE,
    CONF_TCL_ID,
    TCL_BASE_SCHEMA,
    TCL_FV_SCHEMA,
    TclBase,
    tcl_ns,
    tcl_parented_schema,
)

AUTO_LOAD = ["tcl", "climate"]
CODEOWNERS = ["@I-am-nightingale", "@xaxexa", "@junkfix"]
DEPENDENCIES = ["climate", "uart"]

TCLAC_MIN_TEMPERATURE = 16.0
TCLAC_MAX_TEMPERATURE = 31.0
TCLAC_TARGET_TEMPERATURE_STEP = 1.0
TCLAC_CURRENT_TEMPERATURE_STEP = 1.0

CONF_VERTICAL_AIRFLOW = "vertical_airflow"
CONF_HORIZONTAL_AIRFLOW = "horizontal_airflow"
CONF_VERTICAL_SWING_MODE = "vertical_swing_mode"
CONF_HORIZONTAL_SWING_MODE = "horizontal_swing_mode"


TclClimate = tcl_ns.class_("TclClimate", TclBase, climate.Climate)


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
            cv.Optional(CONF_BEEPER): cv.boolean,
            cv.Optional(CONF_DISPLAY): cv.boolean,
            cv.Optional(CONF_FORCE): cv.boolean,
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
    ).extend(TCL_BASE_SCHEMA),
    validate_visual,
)

FINAL_VALIDATE_SCHEMA = TCL_FV_SCHEMA

VerticalAirflowAction = tcl_ns.class_("VerticalAirflowAction", automation.Action)
HorizontalAirflowAction = tcl_ns.class_("HorizontalAirflowAction", automation.Action)
VerticalSwingDirectionAction = tcl_ns.class_(
    "VerticalSwingDirectionAction", automation.Action
)
HorizontalSwingDirectionAction = tcl_ns.class_(
    "HorizontalSwingDirectionAction", automation.Action
)


def templatize(value):
    if isinstance(value, cv.Schema):
        value = value.schema
    ret = {}
    for key, val in value.items():
        ret[key] = cv.templatable(val)
    return cv.Schema(ret)


def register_action(name, type_, schema):
    validator = templatize(schema).extend(tcl_parented_schema(TclClimate))
    if len(schema) == 1:
        validator = automation.maybe_conf(next(iter(schema.schema)), validator)
    registerer = automation.register_action(f"tcl_climate.{name}", type_, validator)

    def decorator(func):
        async def new_func(config, action_id, template_arg, args):
            var = cg.new_Pvariable(action_id, template_arg)
            await cg.register_parented(var, config[CONF_TCL_ID])
            await coroutine(func)(var, config, args)
            return var

        return registerer(new_func)

    return decorator


# Регистрация события установки вертикальной фиксации заслонки
@register_action(
    "set_vertical_airflow",
    VerticalAirflowAction,
    {
        cv.Required(CONF_VERTICAL_AIRFLOW): cv.enum(
            AIRFLOW_VERTICAL_DIRECTION_OPTIONS,
            upper=True,
        )
    },
)
async def tclac_set_vertical_airflow_to_code(var, config, args):
    template_ = await cg.templatable(
        config[CONF_VERTICAL_AIRFLOW], args, AirflowVerticalDirection
    )
    cg.add(var.set_direction(template_))


# Регистрация события установки горизонтальной фиксации заслонок
@register_action(
    "set_horizontal_airflow",
    HorizontalAirflowAction,
    {
        cv.Required(CONF_HORIZONTAL_AIRFLOW): cv.enum(
            AIRFLOW_HORIZONTAL_DIRECTION_OPTIONS,
            upper=True,
        )
    },
)
async def tclac_set_horizontal_airflow_to_code(var, config, args):
    template_ = await cg.templatable(
        config[CONF_HORIZONTAL_AIRFLOW], args, AirflowHorizontalDirection
    )
    cg.add(var.set_direction(template_))


# Регистрация события установки вертикального качания шторки
@register_action(
    "set_vertical_swing_direction",
    VerticalSwingDirectionAction,
    {
        cv.Required(CONF_VERTICAL_SWING_MODE): cv.enum(
            VERTICAL_SWING_DIRECTION_OPTIONS,
            upper=True,
        ),
    },
)
async def tclac_set_vertical_swing_direction_to_code(var, config, args):
    template_ = await cg.templatable(
        config[CONF_VERTICAL_SWING_MODE], args, VerticalSwingDirection
    )
    cg.add(var.set_swing_direction(template_))


# Регистрация события установки горизонтального качания шторок
@register_action(
    "set_horizontal_swing_direction",
    HorizontalSwingDirectionAction,
    {
        cv.Required(CONF_HORIZONTAL_SWING_MODE): cv.enum(
            HORIZONTAL_SWING_DIRECTION_OPTIONS,
            upper=True,
        ),
    },
)
async def tclac_set_horizontal_swing_direction_to_code(var, config, args):
    template_ = await cg.templatable(
        config[CONF_HORIZONTAL_SWING_MODE], args, HorizontalSwingDirection
    )
    cg.add(var.set_swing_direction(template_))


# Добавление конфигурации в код
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    await climate.register_climate(var, config)

    if CONF_BEEPER in config:
        cg.add(var.set_beeper_state(config[CONF_BEEPER]))
    if CONF_DISPLAY in config:
        cg.add(var.set_display_state(config[CONF_DISPLAY]))
    if CONF_FORCE in config:
        cg.add(var.set_force_state(config[CONF_FORCE]))
    if CONF_SUPPORTED_MODES in config:
        cg.add(var.set_supported_modes(config[CONF_SUPPORTED_MODES]))
    if CONF_SUPPORTED_PRESETS in config:
        cg.add(var.set_supported_presets(config[CONF_SUPPORTED_PRESETS]))
    if CONF_SUPPORTED_FAN_MODES in config:
        cg.add(var.set_supported_fan_modes(config[CONF_SUPPORTED_FAN_MODES]))
    if CONF_SUPPORTED_SWING_MODES in config:
        cg.add(var.set_supported_swing_modes(config[CONF_SUPPORTED_SWING_MODES]))
