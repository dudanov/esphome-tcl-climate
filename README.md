Внешний компонент кондиционеров TCL и аналогов для Home Assistant, используя ESPHome.
Поддерживаются кондиционеры типа TAC-07CHSA и подобные. Увы, предположить точно получится подключить кондиционер или нет практически
невозможно из-за огромного разбега в комплектациях: даже одна и та же модель, буквально буква-в-букву может, например, не иметь
родного модуля WiFI, не иметь провода с USB разъемом или вовсе на плате управления может не быть впаян разъем UART.
Однако, в целом, с пайкой или без, проверены следующие кондиционеры:
- TCL TAC-07CHSA/TPG-W
- TCL TAC-09CHSA/TPG
- TCL TAC-12CHSA/TPG
- TCL TAC-09CHSA/DSEI-W
- Daichi AIR20AVQ1/AIR20FV1
____
Пост по проекту находится [в моем канале на Дзене](https://dzen.ru/b/Zhmd3bLEd3GbU8mD)

Это хоть и не готовый вариант, но вполне себе образец для бета-тестирования, есть уже почти все, что хотелось добавить и это все
вполне неплохо и довольно корректно работает. А вот на сколько неплохо и корректно- это и предстоит выяснить.
Используя компонент прямо сейчас Вы уже не рискуете душевным здоровьем, но внезапные глюки вполне могут напасть. Если вдруг такое
случиться именно с Вами- прошу сообщить мне на Дзене, приму меры.
Я все еще не оставляю подробного описания, уповая на то, что проверять компонент и проводить эксперименты будут сколь-нибудь
подкованные в этом деле люди, не боящиеся наброситься с палкой на багованного медведя. Можно ли этим пользоваться "на повседневку"?
Можно. Но лучше подождать окончательного варианта- оно ведь так надежнее.
____
Образец для конфигурации ESPHome в файле TCL-Conditioner.yaml. Скачайте к себе и используйте в ESPHome, или просто скопируйте из него
всю конфигурацию и вставьте вместо своей, однако, не забыв отредактировать все поля. В файле есть подсказки по каждому полю.

Вопрос может возникнуть с 2 моментами: платформа (чип или модуль) и подгружаемые файлы. Попробую объяснить.

## Настройка платформы
Платформа настраивается точно так же, как ей и полагается настраиваться в ESPHome. Например, так выглядит кусок кода для ESP-01S:
```yaml
esp8266:
  board: esp01_1m
```
А вот так выглядит кусок кода для модуля Hommyn HDN/WFN-02-01 из первой статьи про кондиционер:
```yaml
esp32:
  board: esp32-c3-devkitm-1
  framework:
    type: arduino
```
Можно подключать платформу и через основной конфиг, вот, предложенный [испытателем альфа-версии](https://github.com/kai-zer-ru), пример для Esp32 WROOM32:
```yaml
esphome:
  platform: ESP32
  board: nodemcu-32s
```
В общем- все то же самое, как и обычно, вариант под свою платформу легко ищется в интернете.

**!Важно не забыть закомментировать или удалить строки других платформ!**

## Настройка подгружаемых файлов
Для добавления или удаления определенных частей конфига я решил использовать подгружаемые файлы- они загружаются ESPHome автоматически,
если у сервера с Home Assistant есть доступ в интернет. Такой подход позволяет редактировать и обновлять не весь конфиг куском,
а частями, не трогая то, что работает.
Еще один плюс- не нужно километровые куски кода комментировать или раскомментировать, не нужно знать разметку, нет необходимости считать
проклятые пробелы и прочее. Все делается добавлением или удалением ссылок на файлы. Итак, вот так выглядит блок подгружаемых файлов:
```yaml
packages:
  remote_package:
    url: https://github.com/I-am-nightingale/tclac.git
    ref: master
    files:
    # v - равнение строк с опциями вот по этой позиции, иначе глючить будет
      - packages/core.yaml # Ядро всего сущего
      # - packages/leds.yaml
    refresh: 30s
```
Все подгружаемые файлы указываются в секции **files:**. Для работы необходимо, чтобы был хотя-бы
```yaml
- packages/core.yaml # Ядро всего сущего
```
Все остальные модули по желанию (их описание в том же файле чуть выше). **Важно**, чтобы все строки с файлами были выровнены по
импровизированной метке, которую я специально указал, иначе у ESPHome возникнет много вопросов к Вам. Например, **должно быть так:**
```yaml
packages:
  remote_package:
    url: https://github.com/I-am-nightingale/tclac.git
    ref: master
    files:
    # v - равнение строк с опциями вот по этой позиции, иначе глючить будет
      - packages/core.yaml # Ядро всего сущего
      - packages/leds.yaml
    refresh: 30s
```
А вот так уже **не правильно:**
```yaml
packages:
  remote_package:
    url: https://github.com/I-am-nightingale/tclac.git
    ref: master
    files:
    # v - равнение строк с опциями вот по этой позиции, иначе глючить будет
      - packages/core.yaml # Ядро всего сущего
        - packages/leds.yaml
    refresh: 30s
```