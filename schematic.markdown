# RetroWave DinoDash Wiring Schematic

## Components
- Arduino Uno
- SSD1306 OLED (128x64, I2C)
- HC-SR04 Ultrasonic Sensor
- Passive Buzzer (2-pin)

## Wiring
| Arduino Pin | Component Pin |
|-------------|---------------|
| 5V          | OLED VCC      |
| GND         | OLED GND      |
| GND         | HC-SR04 GND   |
| GND         | Buzzer -      |
| A4          | OLED SDA      |
| A5          | OLED SCL      |
| 9           | HC-SR04 TRIG  |
| 10          | HC-SR04 ECHO  |
| 5           | Buzzer +      |

## Diagram
![Schematic](schematic.png)

## Notes
- Use a stable 5V power source (USB or external).
- Ensure HC-SR04 is unobstructed for accurate distance readings.
- Buzzer polarity: + to pin 5, - to GND.