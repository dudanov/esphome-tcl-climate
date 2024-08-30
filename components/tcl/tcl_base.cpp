/**
 * Create by Miguel Ángel López on 20/07/19
 * and modify by xaxexa
 * Refactoring & component making:
 * Соловей с паяльником 15.03.2024
 **/

#include "tcl_base.h"

namespace esphome {
namespace tcl {

const char *const TAG = "tcl";

#ifdef USE_SWITCH
#define SET_STATE_FUN(name) \
  void TclBase::set_##name##_state(bool state) { \
    this->name##_state_ = state; \
    this->takeControl(); \
    if (this->name##_switch_ != nullptr) \
      this->name##_switch_->publish_state(state); \
  }
#else
#define SET_STATE_FUN(name) \
  void TclBase::set_##name##_state(bool state) { \
    this->name##_state_ = state; \
    this->takeControl(); \
  }
#endif

void TclBase::takeControl() {
  if (this->force_state_ && this->allow_take_control_)
    this->control_();
}

SET_STATE_FUN(beeper)
SET_STATE_FUN(display)
SET_STATE_FUN(force)

}  // namespace tcl
}  // namespace esphome
