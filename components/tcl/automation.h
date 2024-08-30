#pragma once

#include "tcl_base.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace tcl {

template<typename... Ts> class BaseAction : public Action<Ts...>, public Parented<TclClimate> {};

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
  void play(Ts... x) { this->parent_->set_force_state(true); }
};

// Шаблон действия: выключение принудительного применения настроек
template<typename... Ts> class ForceOffAction : public BaseAction<Ts...> {
  void play(Ts... x) { this->parent_->set_force_state(false); }
};

}  // namespace tcl
}  // namespace esphome
