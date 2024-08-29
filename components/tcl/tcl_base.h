#pragma once

#include "esphome/core/defines.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace tcl {

class TclBase : public esphome::uart::UARTDevice, public PollingComponent {
 public:
  void set_beeper_state(bool state) { this->beeper_status_ = state; }
  void set_display_state(bool state) { this->display_status_ = state; }
  void set_force_state(bool state) { this->force_mode_status_ = state; }

 protected:
  bool beeper_status_{};
  bool display_status_{};
  bool force_mode_status_{};
};

}  // namespace tcl
}  // namespace esphome
