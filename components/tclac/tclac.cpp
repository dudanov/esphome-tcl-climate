/**
 * Create by Miguel Ángel López on 20/07/19
 * and modify by xaxexa
 * Refactoring & component making:
 * Соловей с паяльником 15.03.2024
 **/

#include "tclac.h"
#include "esphome/core/defines.h"

namespace esphome {
namespace tclac {

const char *const TAG = "tclac";

const uint8_t TclClimate::poll[8] = {0xBB, 0x00, 0x01, 0x04, 0x02, 0x01, 0x00, 0xBD};

ClimateTraits TclClimate::traits() {
  auto traits = climate::ClimateTraits();

  traits.set_supports_action(false);
  traits.set_supports_current_temperature(true);
  traits.set_supports_two_point_target_temperature(false);

  traits.set_supported_modes(this->supported_modes_);
  traits.set_supported_presets(this->supported_presets_);
  traits.set_supported_fan_modes(this->supported_fan_modes_);
  traits.set_supported_swing_modes(this->supported_swing_modes_);

  traits.add_supported_mode(climate::CLIMATE_MODE_OFF);  // Выключенный режим кондиционера доступен всегда
  traits.add_supported_mode(climate::CLIMATE_MODE_AUTO);  // Автоматический режим кондиционера тоже
  traits.add_supported_fan_mode(climate::CLIMATE_FAN_AUTO);  // Автоматический режим вентилятора доступен всегда
  traits.add_supported_swing_mode(climate::CLIMATE_SWING_OFF);  // Выключенный режим качания заслонок доступен всегда
  traits.add_supported_preset(ClimatePreset::CLIMATE_PRESET_NONE);  // На всякий случай без предустановок

  return traits;
}

void TclClimate::setup() {}

void TclClimate::loop() {
  if (!esphome::uart::UARTDevice::available())
    return;

  // В буфере UART что-то есть, то читаем это что-то

  dataRX[0] = esphome::uart::UARTDevice::read();

  // Если принятый байт- не заголовок (0xBB), то просто покидаем цикл
  if (dataRX[0] != 0xBB) {
    ESP_LOGD(TAG, "Wrong byte");
    return;
  }

  // А вот если совпал заголовок (0xBB), то начинаем чтение по цепочке еще 4 байт
  delay(5);
  dataRX[1] = esphome::uart::UARTDevice::read();
  delay(5);
  dataRX[2] = esphome::uart::UARTDevice::read();
  delay(5);
  dataRX[3] = esphome::uart::UARTDevice::read();
  delay(5);
  dataRX[4] = esphome::uart::UARTDevice::read();

  // Из первых 5 байт нам нужен пятый- он содержит длину сообщения
  read_array(dataRX + 5, dataRX[4] + 1);

  ESP_LOGD(TAG, "RX: %s", format_hex_pretty(dataRX, sizeof(dataRX)).c_str());

  // Проверяем контрольную сумму
  if (!getChecksum(dataRX, sizeof(dataRX))) {
    // Прочитав все из буфера приступаем к разбору данных
    TclClimate::readData();

    return;
  }

  ESP_LOGD(TAG, "Invalid checksum.");
}

void TclClimate::update() {
  this->write_array(poll, sizeof(poll));
  ESP_LOGD(TAG, "TX: %s", format_hex_pretty(poll, sizeof(poll)));
}

void TclClimate::readData() {
  this->current_temperature = fahrenheit_to_celsius(encode_uint16(dataRX[17], dataRX[18]) / 374.0f);
  this->target_temperature = (dataRX[FAN_SPEED_POS] & SET_TEMP_MASK) + 16;

  if (dataRX[MODE_POS] & (1 << 4)) {
    // Если кондиционер включен, то разбираем данные для отображения
    uint8_t modeswitch = MODE_MASK & dataRX[MODE_POS];
    uint8_t fanspeedswitch = FAN_SPEED_MASK & dataRX[FAN_SPEED_POS];
    uint8_t swingmodeswitch = SWING_MODE_MASK & dataRX[SWING_POS];

    switch (modeswitch) {
      case MODE_AUTO:
        mode = climate::CLIMATE_MODE_AUTO;
        break;

      case MODE_COOL:
        mode = climate::CLIMATE_MODE_COOL;
        break;

      case MODE_DRY:
        mode = climate::CLIMATE_MODE_DRY;
        break;

      case MODE_FAN_ONLY:
        mode = climate::CLIMATE_MODE_FAN_ONLY;
        break;

      case MODE_HEAT:
        mode = climate::CLIMATE_MODE_HEAT;
        break;

      default:
        mode = climate::CLIMATE_MODE_AUTO;
    }

    if (dataRX[FAN_QUIET_POS] & FAN_QUIET) {
      fan_mode = climate::CLIMATE_FAN_QUIET;

    } else if (dataRX[MODE_POS] & FAN_DIFFUSE) {
      fan_mode = climate::CLIMATE_FAN_DIFFUSE;

    } else {
      switch (fanspeedswitch) {
        case FAN_AUTO:
          fan_mode = climate::CLIMATE_FAN_AUTO;
          break;
        case FAN_LOW:
          fan_mode = climate::CLIMATE_FAN_LOW;
          break;
        case FAN_MIDDLE:
          fan_mode = climate::CLIMATE_FAN_MIDDLE;
          break;
        case FAN_MEDIUM:
          fan_mode = climate::CLIMATE_FAN_MEDIUM;
          break;
        case FAN_HIGH:
          fan_mode = climate::CLIMATE_FAN_HIGH;
          break;
        case FAN_FOCUS:
          fan_mode = climate::CLIMATE_FAN_FOCUS;
          break;
        default:
          fan_mode = climate::CLIMATE_FAN_AUTO;
      }
    }

    switch (swingmodeswitch) {
      case SWING_OFF:
        swing_mode = climate::CLIMATE_SWING_OFF;
        break;
      case SWING_HORIZONTAL:
        swing_mode = climate::CLIMATE_SWING_HORIZONTAL;
        break;
      case SWING_VERTICAL:
        swing_mode = climate::CLIMATE_SWING_VERTICAL;
        break;
      case SWING_BOTH:
        swing_mode = climate::CLIMATE_SWING_BOTH;
        break;
    }

    // Обработка данных о пресете
    preset = ClimatePreset::CLIMATE_PRESET_NONE;
    if (dataRX[7] & (1 << 6)) {
      preset = ClimatePreset::CLIMATE_PRESET_ECO;
    } else if (dataRX[9] & (1 << 2)) {
      preset = ClimatePreset::CLIMATE_PRESET_COMFORT;
    } else if (dataRX[19] & (1 << 0)) {
      preset = ClimatePreset::CLIMATE_PRESET_SLEEP;
    }

  } else {
    // Если кондиционер выключен, то все режимы показываются, как выключенные
    this->mode = climate::CLIMATE_MODE_OFF;
    // fan_mode = climate::CLIMATE_FAN_OFF;
    this->swing_mode = climate::CLIMATE_SWING_OFF;
    this->preset = ClimatePreset::CLIMATE_PRESET_NONE;
  }

  // Публикуем данные
  this->publish_state();
  allow_take_control = true;
}

// Climate control
void TclClimate::control(const ClimateCall &call) {
  // Запрашиваем данные из переключателя режимов работы кондиционера
  if (call.get_mode().has_value()) {
    switch_climate_mode = call.get_mode().value();
    ESP_LOGD(TAG, "Get MODE from call");

  } else {
    switch_climate_mode = mode;
    ESP_LOGD(TAG, "Get MODE from AC");
  }

  // Запрашиваем данные из переключателя предустановок кондиционера
  if (call.get_preset().has_value()) {
    switch_preset = call.get_preset().value();

  } else {
    switch_preset = preset.value();
  }

  // Запрашиваем данные из переключателя режимов вентилятора
  if (call.get_fan_mode().has_value()) {
    switch_fan_mode = call.get_fan_mode().value();

  } else {
    switch_fan_mode = fan_mode.value();
  }

  // Запрашиваем данные из переключателя режимов качания заслонок
  if (call.get_swing_mode().has_value()) {
    switch_swing_mode = call.get_swing_mode().value();

  } else {
    // А если в переключателе пусто- заполняем значением из последнего опроса состояния. Типа, ничего не поменялось.
    switch_swing_mode = swing_mode;
  }

  // Расчет температуры
  if (call.get_target_temperature().has_value()) {
    target_temperature_set = 31 - (int) call.get_target_temperature().value();

  } else {
    target_temperature_set = 31 - (int) target_temperature;
  }

  is_call_control = true;
  takeControl();
  allow_take_control = true;
}

void TclClimate::takeControl() {
  dataTX[7] = 0b00000000;
  dataTX[8] = 0b00000000;
  dataTX[9] = 0b00000000;
  dataTX[10] = 0b00000000;
  dataTX[11] = 0b00000000;
  dataTX[19] = 0b00000000;
  dataTX[32] = 0b00000000;
  dataTX[33] = 0b00000000;

  if (is_call_control != true) {
    ESP_LOGD(TAG, "Get MODE from AC for force config");
    switch_climate_mode = mode;
    switch_preset = preset.value();
    switch_fan_mode = fan_mode.value();
    switch_swing_mode = swing_mode;
    target_temperature_set = 31 - (int) target_temperature;
  }

  // Включаем или отключаем пищалку в зависимости от переключателя в настройках
  if (beeper_status_) {
    ESP_LOGD(TAG, "Beep mode ON");
    dataTX[7] += 0b00100000;
  } else {
    ESP_LOGD(TAG, "Beep mode OFF");
    dataTX[7] += 0b00000000;
  }

  // Включаем или отключаем дисплей на кондиционере в зависимости от переключателя в настройках
  // Включаем дисплей только если кондиционер в одном из рабочих режимов

  // ВНИМАНИЕ! При выключении дисплея кондиционер сам принудительно переходит в автоматический режим!

  if ((display_status_) && (switch_climate_mode != climate::CLIMATE_MODE_OFF)) {
    ESP_LOGD(TAG, "Dispaly turn ON");
    dataTX[7] += 0b01000000;
  } else {
    ESP_LOGD(TAG, "Dispaly turn OFF");
    dataTX[7] += 0b00000000;
  }

  // Настраиваем режим работы кондиционера
  switch (switch_climate_mode) {
    case climate::CLIMATE_MODE_OFF:
      dataTX[7] += 0b00000000;
      dataTX[8] += 0b00000000;
      break;
    case climate::CLIMATE_MODE_AUTO:
      dataTX[7] += 0b00000100;
      dataTX[8] += 0b00001000;
      break;
    case climate::CLIMATE_MODE_COOL:
      dataTX[7] += 0b00000100;
      dataTX[8] += 0b00000011;
      break;
    case climate::CLIMATE_MODE_DRY:
      dataTX[7] += 0b00000100;
      dataTX[8] += 0b00000010;
      break;
    case climate::CLIMATE_MODE_FAN_ONLY:
      dataTX[7] += 0b00000100;
      dataTX[8] += 0b00000111;
      break;
    case climate::CLIMATE_MODE_HEAT:
      dataTX[7] += 0b00000100;
      dataTX[8] += 0b00000001;
      break;
  }

  // Настраиваем режим вентилятора
  switch (switch_fan_mode) {
    case climate::CLIMATE_FAN_AUTO:
      dataTX[8] += 0b00000000;
      dataTX[10] += 0b00000000;
      break;
    case climate::CLIMATE_FAN_QUIET:
      dataTX[8] += 0b10000000;
      dataTX[10] += 0b00000000;
      break;
    case climate::CLIMATE_FAN_LOW:
      dataTX[8] += 0b00000000;
      dataTX[10] += 0b00000001;
      break;
    case climate::CLIMATE_FAN_MIDDLE:
      dataTX[8] += 0b00000000;
      dataTX[10] += 0b00000110;
      break;
    case climate::CLIMATE_FAN_MEDIUM:
      dataTX[8] += 0b00000000;
      dataTX[10] += 0b00000011;
      break;
    case climate::CLIMATE_FAN_HIGH:
      dataTX[8] += 0b00000000;
      dataTX[10] += 0b00000111;
      break;
    case climate::CLIMATE_FAN_FOCUS:
      dataTX[8] += 0b00000000;
      dataTX[10] += 0b00000101;
      break;
    case climate::CLIMATE_FAN_DIFFUSE:
      dataTX[8] += 0b01000000;
      dataTX[10] += 0b00000000;
      break;
  }

  // Устанавливаем режим качания заслонок
  switch (switch_swing_mode) {
    case climate::CLIMATE_SWING_OFF:
      dataTX[10] += 0b00000000;
      dataTX[11] += 0b00000000;
      break;
    case climate::CLIMATE_SWING_VERTICAL:
      dataTX[10] += 0b00111000;
      dataTX[11] += 0b00000000;
      break;
    case climate::CLIMATE_SWING_HORIZONTAL:
      dataTX[10] += 0b00000000;
      dataTX[11] += 0b00001000;
      break;
    case climate::CLIMATE_SWING_BOTH:
      dataTX[10] += 0b00111000;
      dataTX[11] += 0b00001000;
      break;
  }

  // Устанавливаем предустановки кондиционера
  switch (switch_preset) {
    case ClimatePreset::CLIMATE_PRESET_NONE:
      break;
    case ClimatePreset::CLIMATE_PRESET_ECO:
      dataTX[7] += 0b10000000;
      break;
    case ClimatePreset::CLIMATE_PRESET_SLEEP:
      dataTX[19] += 0b00000001;
      break;
    case ClimatePreset::CLIMATE_PRESET_COMFORT:
      dataTX[8] += 0b00010000;
      break;
  }

  // Режим заслонок
  //	Вертикальная заслонка
  //		Качание вертикальной заслонки [10 байт, маска 00111000]:
  //			000 - Качание отключено, заслонка в последней позиции или в фиксации
  //			111 - Качание включено в выбранном режиме
  //		Режим качания вертикальной заслонки (режим фиксации заслонки роли не играет, если качание включено) [32 байт,
  // маска 00011000]: 			01 - качание сверху вниз, ПО УМОЛЧАНИЮ 			10 - качание в верхней половине 			11 -
  // качание в нижней половине 		Режим фиксации заслонки (режим качания заслонки роли не играет, если качание
  // выключено) [32 байт, маска 00000111]: 			000 - нет фиксации, ПО УМОЛЧАНИЮ 			001 - фиксация вверху 			010 -
  // фиксация между верхом и серединой 			011 - фиксация в середине 			100 - фиксация между серединой и низом
  // 101 - фиксация внизу 	Горизонтальные заслонки 		Качание горизонтальных заслонок [11 байт, маска 00001000]: 0 -
  // Качание отключено, заслонки в последней позиции или в фиксации 			1 - Качание включено в выбранном режиме
  // Режим качания горизонтальных заслонок (режим фиксации заслонок роли не играет, если качание включено) [33 байт,
  // маска 00111000]: 			001 - качание слева направо, ПО УМОЛЧАНИЮ 			010 - качание слева 			011 - качание по
  // середине 			100 - качание справа 		Режим фиксации горизонтальных заслонок (режим качания заслонок роли не
  // играет, если качание выключено) [33 байт, маска 00000111]: 			000 - нет фиксации, ПО УМОЛЧАНИЮ 			001 -
  // фиксация слева 			010 - фиксация между левой стороной и серединой 			011 - фиксация в середине 			100 -
  // фиксация между серединой и правой стороной 			101 - фиксация справа

  // Устанавливаем режим для качания вертикальной заслонки
  switch (vertical_swing_direction_) {
    case VerticalSwingDirection::UP_DOWN:
      dataTX[32] += 0b00001000;
      ESP_LOGD(TAG, "Vertical swing: up-down");
      break;
    case VerticalSwingDirection::UPSIDE:
      dataTX[32] += 0b00010000;
      ESP_LOGD(TAG, "Vertical swing: upper");
      break;
    case VerticalSwingDirection::DOWNSIDE:
      dataTX[32] += 0b00011000;
      ESP_LOGD(TAG, "Vertical swing: downer");
      break;
  }
  // Устанавливаем режим для качания горизонтальных заслонок
  switch (horizontal_swing_direction_) {
    case HorizontalSwingDirection::LEFT_RIGHT:
      dataTX[33] += 0b00001000;
      ESP_LOGD(TAG, "Horizontal swing: left-right");
      break;
    case HorizontalSwingDirection::LEFTSIDE:
      dataTX[33] += 0b00010000;
      ESP_LOGD(TAG, "Horizontal swing: lefter");
      break;
    case HorizontalSwingDirection::CENTER:
      dataTX[33] += 0b00011000;
      ESP_LOGD(TAG, "Horizontal swing: center");
      break;
    case HorizontalSwingDirection::RIGHTSIDE:
      dataTX[33] += 0b00100000;
      ESP_LOGD(TAG, "Horizontal swing: righter");
      break;
  }
  // Устанавливаем положение фиксации вертикальной заслонки
  switch (vertical_direction_) {
    case AirflowVerticalDirection::LAST:
      dataTX[32] += 0b00000000;
      ESP_LOGD(TAG, "Vertical fix: last position");
      break;
    case AirflowVerticalDirection::MAX_UP:
      dataTX[32] += 0b00000001;
      ESP_LOGD(TAG, "Vertical fix: up");
      break;
    case AirflowVerticalDirection::UP:
      dataTX[32] += 0b00000010;
      ESP_LOGD(TAG, "Vertical fix: upper");
      break;
    case AirflowVerticalDirection::CENTER:
      dataTX[32] += 0b00000011;
      ESP_LOGD(TAG, "Vertical fix: center");
      break;
    case AirflowVerticalDirection::DOWN:
      dataTX[32] += 0b00000100;
      ESP_LOGD(TAG, "Vertical fix: downer");
      break;
    case AirflowVerticalDirection::MAX_DOWN:
      dataTX[32] += 0b00000101;
      ESP_LOGD(TAG, "Vertical fix: down");
      break;
  }
  // Устанавливаем положение фиксации горизонтальных заслонок
  switch (horizontal_direction_) {
    case AirflowHorizontalDirection::LAST:
      dataTX[33] += 0b00000000;
      ESP_LOGD(TAG, "Horizontal fix: last position");
      break;
    case AirflowHorizontalDirection::MAX_LEFT:
      dataTX[33] += 0b00000001;
      ESP_LOGD(TAG, "Horizontal fix: left");
      break;
    case AirflowHorizontalDirection::LEFT:
      dataTX[33] += 0b00000010;
      ESP_LOGD(TAG, "Horizontal fix: lefter");
      break;
    case AirflowHorizontalDirection::CENTER:
      dataTX[33] += 0b00000011;
      ESP_LOGD(TAG, "Horizontal fix: center");
      break;
    case AirflowHorizontalDirection::RIGHT:
      dataTX[33] += 0b00000100;
      ESP_LOGD(TAG, "Horizontal fix: righter");
      break;
    case AirflowHorizontalDirection::MAX_RIGHT:
      dataTX[33] += 0b00000101;
      ESP_LOGD(TAG, "Horizontal fix: right");
      break;
  }

  // Установка температуры
  dataTX[9] = target_temperature_set;

  // Собираем массив байт для отправки в кондиционер
  dataTX[0] = 0xBB;  // стартовый байт заголовка
  dataTX[1] = 0x00;  // стартовый байт заголовка
  dataTX[2] = 0x01;  // стартовый байт заголовка
  dataTX[3] = 0x03;  // 0x03 - управление, 0x04 - опрос
  dataTX[4] = 0x20;  // 0x20 - управление, 0x19 - опрос
  dataTX[5] = 0x03;  //??
  dataTX[6] = 0x01;  //??
  // dataTX[7] = 0x64;	//eco,display,beep,ontimerenable, offtimerenable,power,0,0
  // dataTX[8] = 0x08;	//mute,0,turbo,health, mode(4) mode 01 heat, 02 dry, 03 cool, 07 fan, 08 auto, health(+16),
  // 41=turbo-heat 43=turbo-cool (turbo = 0x40+ 0x01..0x08) dataTX[9] = 0x0f;	//0 -31 ;    15 - 16 0,0,0,0, temp(4)
  // settemp 31 - x dataTX[10] = 0x00;	//0,timerindicator,swingv(3),fan(3) fan+swing modes //0=auto 1=low 2=med 3=high
  // dataTX[11] = 0x00;	//0,offtimer(6),0
  dataTX[12] = 0x00;  // fahrenheit,ontimer(6),0 cf 80=f 0=c
  dataTX[13] = 0x01;  //??
  dataTX[14] = 0x00;  // 0,0,halfdegree,0,0,0,0,0
  dataTX[15] = 0x00;  //??
  dataTX[16] = 0x00;  //??
  dataTX[17] = 0x00;  //??
  dataTX[18] = 0x00;  //??
  // dataTX[19] = 0x00;	//sleep on = 1 off=0
  dataTX[20] = 0x00;  //??
  dataTX[21] = 0x00;  //??
  dataTX[22] = 0x00;  //??
  dataTX[23] = 0x00;  //??
  dataTX[24] = 0x00;  //??
  dataTX[25] = 0x00;  //??
  dataTX[26] = 0x00;  //??
  dataTX[27] = 0x00;  //??
  dataTX[28] = 0x00;  //??
  dataTX[30] = 0x00;  //??
  dataTX[31] = 0x00;  //??
  // dataTX[32] = 0x00;	//0,0,0,режим вертикального качания(2),режим вертикальной фиксации(3)
  // dataTX[33] = 0x00;	//0,0,режим горизонтального качания(3),режим горизонтальной фиксации(3)
  dataTX[34] = 0x00;  //??
  dataTX[35] = 0x00;  //??
  dataTX[36] = 0x00;  //??
  dataTX[37] = 0xFF;  // Контрольная сумма
  dataTX[37] = TclClimate::getChecksum(dataTX, sizeof(dataTX));

  TclClimate::sendData(dataTX, sizeof(dataTX));
  allow_take_control = false;
  is_call_control = false;
}

// Отправка данных в кондиционер
void TclClimate::sendData(uint8_t *message, uint8_t size) {
  // Serial.write(message, size);
  this->esphome::uart::UARTDevice::write_array(message, size);
  // auto raw = getHex(message, size);
  ESP_LOGD(TAG, "Message to TCL sended...");
}

// Вычисление контрольной суммы
uint8_t TclClimate::getChecksum(const uint8_t *msg, size_t size) {
  uint8_t crc = 0;

  do {
    crc ^= *msg++;
  } while (--size);

  return crc;
}

// Действия с данными из конфига

// Получение состояния пищалки
void TclClimate::set_beeper_state(bool state) {
  this->beeper_status_ = state;
  if (force_mode_status_) {
    if (allow_take_control) {
      TclClimate::takeControl();
    }
  }
}
// Получение состояния дисплея кондиционера
void TclClimate::set_display_state(bool state) {
  this->display_status_ = state;
  if (force_mode_status_) {
    if (allow_take_control) {
      TclClimate::takeControl();
    }
  }
}
// Получение состояния режима принудительного применения настроек
void TclClimate::set_force_mode_state(bool state) { this->force_mode_status_ = state; }
// Получение пина светодиода приема данных
#ifdef CONF_RX_LED
void TclClimate::set_rx_led_pin(GPIOPin *rx_led_pin) { this->rx_led_pin_ = rx_led_pin; }
#endif
// Получение пина светодиода передачи данных
#ifdef CONF_TX_LED
void TclClimate::set_tx_led_pin(GPIOPin *tx_led_pin) { this->tx_led_pin_ = tx_led_pin; }
#endif
// Получение состояния светодиодов связи модуля
void TclClimate::set_module_display_state(bool state) { this->module_display_status_ = state; }
// Получение режима фиксации вертикальной заслонки
void TclClimate::set_vertical_airflow(AirflowVerticalDirection direction) {
  this->vertical_direction_ = direction;
  if (force_mode_status_) {
    if (allow_take_control) {
      TclClimate::takeControl();
    }
  }
}
// Получение режима фиксации горизонтальных заслонок
void TclClimate::set_horizontal_airflow(AirflowHorizontalDirection direction) {
  this->horizontal_direction_ = direction;
  if (force_mode_status_) {
    if (allow_take_control) {
      TclClimate::takeControl();
    }
  }
}
// Получение режима качания вертикальной заслонки
void TclClimate::set_vertical_swing_direction(VerticalSwingDirection direction) {
  this->vertical_swing_direction_ = direction;
  if (force_mode_status_) {
    if (allow_take_control) {
      TclClimate::takeControl();
    }
  }
}
// Получение доступных режимов работы кондиционера
void TclClimate::set_supported_modes(const std::set<climate::ClimateMode> &modes) { this->supported_modes_ = modes; }
// Получение режима качания горизонтальных заслонок
void TclClimate::set_horizontal_swing_direction(HorizontalSwingDirection direction) {
  horizontal_swing_direction_ = direction;
  if (force_mode_status_) {
    if (allow_take_control) {
      TclClimate::takeControl();
    }
  }
}
// Получение доступных скоростей вентилятора
void TclClimate::set_supported_fan_modes(const std::set<climate::ClimateFanMode> &modes) {
  this->supported_fan_modes_ = modes;
}
// Получение доступных режимов качания заслонок
void TclClimate::set_supported_swing_modes(const std::set<climate::ClimateSwingMode> &modes) {
  this->supported_swing_modes_ = modes;
}
// Получение доступных предустановок
void TclClimate::set_supported_presets(const std::set<climate::ClimatePreset> &presets) {
  this->supported_presets_ = presets;
}

}  // namespace tclac
}  // namespace esphome