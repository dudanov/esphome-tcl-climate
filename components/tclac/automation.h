#pragma once

#include "tclac.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace tclac {

template<typename... Ts> class TclActionBase : public Action<Ts...>, public Parented<TclClimate> {};

// Шаблон действия: изменение вертикальной фиксации заслонки
template<typename... Ts> class VerticalAirflowAction : public TclActionBase<Ts...> {
  TEMPLATABLE_VALUE(AirflowVerticalDirection, direction)
  void play(Ts... x) { this->parent_->set_vertical_airflow(this->direction_.value(x...)); }
};

// Шаблон действия: изменение горизонтальной фиксации заслонок
template<typename... Ts> class HorizontalAirflowAction : public TclActionBase<Ts...> {
  TEMPLATABLE_VALUE(AirflowHorizontalDirection, direction)
  void play(Ts... x) { this->parent_->set_horizontal_airflow(this->direction_.value(x...)); }
};

// Шаблон действия: изменение режима вертикального качания заслонки
template<typename... Ts> class VerticalSwingDirectionAction : public TclActionBase<Ts...> {
  TEMPLATABLE_VALUE(VerticalSwingDirection, direction)
  void play(Ts... x) { this->parent_->set_vertical_swing_direction(this->direction_.value(x...)); }
};

// Шаблон действия: изменение режима горизонтального качания заслонок
template<typename... Ts> class HorizontalSwingDirectionAction : public TclActionBase<Ts...> {
  TEMPLATABLE_VALUE(HorizontalSwingDirection, direction)
  void play(Ts... x) { this->parent_->set_horizontal_swing_direction(this->direction_.value(x...)); }
};

// Шаблон действия: включение дисплея
template<typename... Ts> class DisplayOnAction : public TclActionBase<Ts...> {
  void play(Ts... x) { this->parent_->set_display_state(true); }
};

// Шаблон действия: выключение дисплея
template<typename... Ts> class DisplayOffAction : public TclActionBase<Ts...> {
  void play(Ts... x) { this->parent_->set_display_state(false); }
};

// Шаблон действия: включение пищалки
template<typename... Ts> class BeeperOnAction : public TclActionBase<Ts...> {
  void play(Ts... x) { this->parent_->set_beeper_state(true); }
};

// Шаблон действия: выклюение пищалки
template<typename... Ts> class BeeperOffAction : public TclActionBase<Ts...> {
  void play(Ts... x) { this->parent_->set_beeper_state(false); }
};

// Шаблон действия: включение индикатора модуля
template<typename... Ts> class ModuleDisplayOnAction : public TclActionBase<Ts...> {
  void play(Ts... x) { this->parent_->set_module_display_state(true); }
};

// Шаблон действия: выключение индикатора модуля
template<typename... Ts> class ModuleDisplayOffAction : public TclActionBase<Ts...> {
  void play(Ts... x) { this->parent_->set_module_display_state(false); }
};

// Шаблон действия: включение принудительного применения настроек
template<typename... Ts> class ForceOnAction : public TclActionBase<Ts...> {
  void play(Ts... x) { this->parent_->set_force_mode_state(true); }
};

// Шаблон действия: выключение принудительного применения настроек
template<typename... Ts> class ForceOffAction : public TclActionBase<Ts...> {
  void play(Ts... x) { this->parent_->set_force_mode_state(false); }
};

}  // namespace tclac
}  // namespace esphome
