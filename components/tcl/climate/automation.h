#pragma once

#include "tcl_climate.h"
#include "esphome/core/automation.h"

#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

namespace esphome {
namespace tcl {

#ifdef USE_SWITCH
class BaseSwitch : public switch_::Switch, public Parented<TclClimate> {};

class BeeperSwitch : public BaseSwitch {
  void write_state(bool state) override {
    this->parent_->set_beeper_state(state);
    this->publish_state(state);
  }
};

class DisplaySwitch : public BaseSwitch {
  void write_state(bool state) override {
    this->parent_->set_display_state(state);
    this->publish_state(state);
  }
};
#endif

template<typename... Ts> class BaseAction : public Action<Ts...>, public Parented<TclClimate> {};

// Шаблон действия: изменение вертикальной фиксации заслонки
template<typename... Ts> class VerticalAirflowAction : public BaseAction<Ts...> {
  TEMPLATABLE_VALUE(AirflowVerticalDirection, direction)
  void play(Ts... x) { this->parent_->set_vertical_airflow(this->direction_.value(x...)); }
};

// Шаблон действия: изменение горизонтальной фиксации заслонок
template<typename... Ts> class HorizontalAirflowAction : public BaseAction<Ts...> {
  TEMPLATABLE_VALUE(AirflowHorizontalDirection, direction)
  void play(Ts... x) { this->parent_->set_horizontal_airflow(this->direction_.value(x...)); }
};

// Шаблон действия: изменение режима вертикального качания заслонки
template<typename... Ts> class VerticalSwingDirectionAction : public BaseAction<Ts...> {
  TEMPLATABLE_VALUE(VerticalSwingDirection, direction)
  void play(Ts... x) { this->parent_->set_vertical_swing_direction(this->direction_.value(x...)); }
};

// Шаблон действия: изменение режима горизонтального качания заслонок
template<typename... Ts> class HorizontalSwingDirectionAction : public BaseAction<Ts...> {
  TEMPLATABLE_VALUE(HorizontalSwingDirection, direction)
  void play(Ts... x) { this->parent_->set_horizontal_swing_direction(this->direction_.value(x...)); }
};

// Шаблон действия: включение дисплея
template<typename... Ts> class DisplayOnAction : public BaseAction<Ts...> {
  void play(Ts... x) { this->parent_->set_display_state(true); }
};

// Шаблон действия: выключение дисплея
template<typename... Ts> class DisplayOffAction : public BaseAction<Ts...> {
  void play(Ts... x) { this->parent_->set_display_state(false); }
};

// Шаблон действия: включение пищалки
template<typename... Ts> class BeeperOnAction : public BaseAction<Ts...> {
  void play(Ts... x) { this->parent_->set_beeper_state(true); }
};

// Шаблон действия: выклюение пищалки
template<typename... Ts> class BeeperOffAction : public BaseAction<Ts...> {
  void play(Ts... x) { this->parent_->set_beeper_state(false); }
};

// Шаблон действия: включение принудительного применения настроек
template<typename... Ts> class ForceOnAction : public BaseAction<Ts...> {
  void play(Ts... x) { this->parent_->set_force_mode_state(true); }
};

// Шаблон действия: выключение принудительного применения настроек
template<typename... Ts> class ForceOffAction : public BaseAction<Ts...> {
  void play(Ts... x) { this->parent_->set_force_mode_state(false); }
};

}  // namespace tcl
}  // namespace esphome
