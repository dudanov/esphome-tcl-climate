#pragma once

#include "esphome/components/switch/switch.h"

#include "../tcl_base.h"

namespace esphome {
namespace tcl {

class BaseSwitch : public switch_::Switch, public Parented<TclBase> {};

class BeeperSwitch : public BaseSwitch {
  void write_state(bool state) override { this->parent_->set_beeper_state(state); }
};

class DisplaySwitch : public BaseSwitch {
  void write_state(bool state) override { this->parent_->set_display_state(state); }
};

class ForceSwitch : public BaseSwitch {
  void write_state(bool state) override { this->parent_->set_force_state(state); }
};

}  // namespace tcl
}  // namespace esphome
