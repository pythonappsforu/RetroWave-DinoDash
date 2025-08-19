# RetroWave DinoDash Setup Guide

## Prerequisites
- Arduino IDE (1.8 or later)
- Libraries: Adafruit_GFX, Adafruit_SSD1306, EEPROM
- Hardware: Arduino Uno, SSD1306 OLED, HC-SR04, passive buzzer, breadboard, wires

## Steps
1. **Install Libraries**:
   - Open Arduino IDE > Sketch > Include Library > Manage Libraries.
   - Search and install `Adafruit_GFX` and `Adafruit_SSD1306`.
   - EEPROM is built-in.
2. **Connect Hardware**:
   - Follow wiring in [schematic.md](schematic.md).
   - Ensure stable 5V power (USB recommended).
3. **Upload Code**:
   - Open `src/DinoRunOLEDClassicDinoEnhanced.ino`.
   - Tools > Board > Arduino Uno.
   - Tools > Port > Select your port.
   - Upload.
4. **Test**:
   - Open Serial Monitor (9600 baud) to debug distance.
   - Wave hand (<15 cm) to start. Check dino animation and sounds.
5. **Customize** (optional):
   - Edit bitmaps in code for new dino designs.
   - Modify sound sequences in `play*Sound` functions.
   - Adjust `obstacleSpeed` or `obstacleGap` for difficulty.

## Debugging
- **OLED Blank**: Run I2C scanner to confirm address (0x3C). Check A4/A5 connections.
- **No Sound**: Test buzzer: `tone(5, 1000, 200); delay(1500);`. Verify pin 5.
- **No Jumps**: Print distance: `Serial.print("Distance: "); Serial.println(getDistance());`. Ensure HC-SR04 is clear.
- **Legs Not Visible**: Slow animation (`lastStep > 400`) or adjust `dinoY`.

## Resources
- [Adafruit SSD1306 Guide](https://learn.adafruit.com/monochrome-oled-breakouts)
- [HC-SR04 Tutorial](https://www.arduino.cc/en/Tutorial/BuiltInExamples/Ping)