#pragma once

#include "../tcl_base.h"

#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

namespace esphome {
namespace tcl {

#ifdef USE_SWITCH
class BaseSwitch : public switch_::Switch, public Parented<TclBase> {};

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

class ForceSwitch : public BaseSwitch {
  void write_state(bool state) override {
    this->parent_->set_force_state(state);
    this->publish_state(state);
  }
};
#endif

}  // namespace tcl
}  // namespace esphome
