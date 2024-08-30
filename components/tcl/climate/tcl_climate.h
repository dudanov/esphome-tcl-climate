/**
 * Create by Miguel Ángel López on 20/07/19
 * and modify by xaxexa
 * Refactoring & component making:
 * Соловей с паяльником 15.03.2024
 */

#pragma once

#include "esphome/core/defines.h"
#include "esphome/components/climate/climate.h"

#include "../tcl_base.h"

namespace esphome {
namespace tcl {

#define SET_TEMP_MASK 0b00001111

#define MODE_POS 7
#define MODE_MASK 0b00111111

#define MODE_AUTO 0b00110101
#define MODE_COOL 0b00110001
#define MODE_DRY 0b00110011
#define MODE_FAN_ONLY 0b00110010
#define MODE_HEAT 0b00110100

#define FAN_SPEED_POS 8
#define FAN_QUIET_POS 33

#define FAN_AUTO 0b10000000        // auto
#define FAN_QUIET 0x80             // silent
#define FAN_LOW 0b10010000         //	|
#define FAN_MIDDLE 0b11000000      //	||
#define FAN_MEDIUM 0b10100000      //	|||
#define FAN_HIGH 0b11010000        //	||||
#define FAN_FOCUS 0b10110000       //	|||||
#define FAN_DIFFUSE 0b10000000     //	POWER [7]
#define FAN_SPEED_MASK 0b11110000  // FAN SPEED MASK

#define SWING_POS 10
#define SWING_OFF 0b00000000
#define SWING_HORIZONTAL 0b00100000
#define SWING_VERTICAL 0b01000000
#define SWING_BOTH 0b01100000
#define SWING_MODE_MASK 0b01100000

using climate::ClimateCall;
using climate::ClimateFanMode;
using climate::ClimateMode;
using climate::ClimatePreset;
using climate::ClimateSwingMode;
using climate::ClimateTraits;

enum class VerticalSwingDirection : uint8_t {
  UP_DOWN,
  UPSIDE,
  DOWNSIDE,
};

enum class HorizontalSwingDirection : uint8_t {
  LEFT_RIGHT,
  LEFTSIDE,
  CENTER,
  RIGHTSIDE,
};

enum AirflowVerticalDirection : uint8_t {
  AFV_LAST,
  AFV_MAX_UP,
  AFV_UP,
  AFV_CENTER,
  AFV_DOWN,
  AFV_MAX_DOWN,
};

enum class AirflowHorizontalDirection : uint8_t {
  LAST,
  MAX_LEFT,
  LEFT,
  CENTER,
  RIGHT,
  MAX_RIGHT,
};

class TclClimate : public TclBase, public climate::Climate {
 public:
  /* Polling component methods. */

  void loop() override;
  void setup() override;
  void update() override;

  void control(const ClimateCall &call) override;


  void set_vertical_airflow(AirflowVerticalDirection direction);

  void set_horizontal_airflow(AirflowHorizontalDirection direction);

  void set_vertical_swing_direction(VerticalSwingDirection direction);

  void set_horizontal_swing_direction(HorizontalSwingDirection direction);

  void set_supported_presets(const std::set<climate::ClimatePreset> &presets);

  void set_supported_modes(std::set<esphome::climate::ClimateMode> modes) { this->supported_modes_ = std::move(modes); }

  void set_supported_fan_modes(std::set<esphome::climate::ClimateFanMode> modes) {
    this->supported_fan_modes_ = std::move(modes);
  }

  void set_supported_swing_modes(std::set<esphome::climate::ClimateSwingMode> modes) {
    this->supported_swing_modes_ = std::move(modes);
  }

 protected:
  // Команда запроса состояния
  static const uint8_t poll[8];

  ClimateTraits traits() override;

  AirflowVerticalDirection vertical_direction_;
  AirflowHorizontalDirection horizontal_direction_;
  VerticalSwingDirection vertical_swing_direction_;
  HorizontalSwingDirection horizontal_swing_direction_;

  std::set<ClimateMode> supported_modes_{};
  std::set<ClimatePreset> supported_presets_{};
  std::set<ClimateFanMode> supported_fan_modes_{};
  std::set<ClimateSwingMode> supported_swing_modes_{};

  uint8_t checksum{};
  // dataTX с управлением состоит из 38 байт
  uint8_t dataTX[38];
  // А dataRX по прежнему из 61 байта
  uint8_t dataRX[61];
  // Инициализация и начальное наполнение переменных состоянй переключателей
  uint8_t switch_preset = 0;
  uint8_t switch_fan_mode = 0;
  bool is_call_control = false;
  uint8_t switch_swing_mode = 0;
  int target_temperature_set = 0;
  uint8_t switch_climate_mode = 0;

  esphome::climate::ClimateTraits traits_;

  void control_() override;
  void readData();
  void sendData(uint8_t *message, uint8_t size);
  static uint8_t getChecksum(const uint8_t *msg, size_t size);
};

}  // namespace tcl
}  // namespace esphome
