#pragma once

#include "esphome/core/defines.h"
#include "esphome/components/uart/uart.h"
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

namespace esphome {
namespace tcl {

class TclBase : public esphome::uart::UARTDevice, public PollingComponent {
 public:
  void set_beeper_state(bool state);
  void set_display_state(bool state);
  void set_force_state(bool state);

#ifdef USE_SWITCH
  SUB_SWITCH(beeper)
  SUB_SWITCH(display)
  SUB_SWITCH(force)
#endif

 protected:
  bool beeper_state_{};
  bool display_state_{};
  bool force_state_{};
  bool allow_take_control_{};

  void takeControl();
  virtual void control_() = 0;
};

}  // namespace tcl
}  // namespace esphome
