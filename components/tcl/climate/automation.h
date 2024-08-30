#pragma once

#include "esphome/core/automation.h"

#include "tcl_climate.h"

namespace esphome {
namespace tcl {

template<typename... Ts> class ClimateAction : public Action<Ts...>, public Parented<TclClimate> {};

// Шаблон действия: изменение вертикальной фиксации заслонки
template<typename... Ts> class VerticalAirflowAction : public ClimateAction<Ts...> {
  TEMPLATABLE_VALUE(AirflowVerticalDirection, direction)
  void play(Ts... x) { this->parent_->set_vertical_airflow(this->direction_.value(x...)); }
};

// Шаблон действия: изменение горизонтальной фиксации заслонок
template<typename... Ts> class HorizontalAirflowAction : public ClimateAction<Ts...> {
  TEMPLATABLE_VALUE(AirflowHorizontalDirection, direction)
  void play(Ts... x) { this->parent_->set_horizontal_airflow(this->direction_.value(x...)); }
};

// Шаблон действия: изменение режима вертикального качания заслонки
template<typename... Ts> class VerticalSwingDirectionAction : public ClimateAction<Ts...> {
  TEMPLATABLE_VALUE(VerticalSwingDirection, direction)
  void play(Ts... x) { this->parent_->set_vertical_swing_direction(this->direction_.value(x...)); }
};

// Шаблон действия: изменение режима горизонтального качания заслонок
template<typename... Ts> class HorizontalSwingDirectionAction : public ClimateAction<Ts...> {
  TEMPLATABLE_VALUE(HorizontalSwingDirection, direction)
  void play(Ts... x) { this->parent_->set_horizontal_swing_direction(this->direction_.value(x...)); }
};

}  // namespace tcl
}  // namespace esphome
