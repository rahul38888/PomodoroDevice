# PomodoroDevice

## 4 x 7 segments display
### 7 segments leds

![7 segments led names](https://github.com/rahul38888/PomodoroDevice/blob/520fe267d27aa2939a2c743a77cb05a0f6bb5576/Media/7_Segments_led_names.png)

### 4x7 segment display pinout diagram

![4x7 segment display pinout diagram](https://github.com/rahul38888/PomodoroDevice/blob/520fe267d27aa2939a2c743a77cb05a0f6bb5576/Media/4-x-7-segment-LED-display-pinout.png)

### Shift register 74HC595

![Shift register 74HC595](https://github.com/rahul38888/PomodoroDevice/blob/91ebc1411ede8d5e7a9827ac29af66c1e5a37e6d/Media/74HC595_pin_diagram.png)

### Pins connections
| 4x7 pins | Arduino pins |
|-|-|
|D1 (Through 220 Ω)|2|
|D2 (Through 220 Ω)|3|
|D3 (Through 220 Ω)|4|
|D4 (Through 220 Ω)|5|

| 4x7 pins | 74HC595 pins |
|-|-|
|A|Q0|
|B|Q1|
|C|Q2|
|D|Q3|
|E|Q4|
|F|Q5|
|G|Q6|
|DP|Q7|

| Arduino pins | 74HC595 pins |
|-|-|
|GND|GND|
|5V|Vcc|
|11|DS|
|GND|OE|
|8|ST_CP|
|12|SH_CP|
|5V|MR|

### Bytes to Character mapping
|Character|Byte value (ABCDEFG)|Integer value|
|-|-|-|
|0|0000001|1|
|1|1001111|79|
|2|0010010|18|
|3|0000110|6|
|4|1001100|76|
|5|0100100|36|
|6|0100000|32|
|7|0001111|15|
|8|0000000|0|
|9|0000100|4|
|Blacnk|1111111|127|

1 means inactive and 0 means active.

**74HC595** need to have 8 bits written into it. The 1st place bit will decide state of **DP**. That is being added on runtime.
