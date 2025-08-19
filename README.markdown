# RetroWave DinoDash

An Arduino remake of the Chrome Dino game with a pixelated T-Rex, touchless ultrasonic controls, and chiptune-inspired sounds. Wave your hand to make the dino jump over cacti and birds, enjoy retro animations, and aim for a high score!

## Demo

\[Insert YouTube Video Link Here\]

## Features

- **Pixelated T-Rex**: Classic Chrome Dino look with clear running leg animation.
- **Touchless Controls**: Jump by waving your hand (&lt;15 cm) using an HC-SR04 ultrasonic sensor.
- **Chiptune Sounds**:
  - Walking: 600/800 Hz alternating steps.
  - Jump: 900–1300 Hz arpeggio.
  - Level-Up: 1200–1800 Hz melody (every 5 avoids).
  - Game Over: 600–150 Hz descending tones.
  - Menu/Retry: 1200–1500 Hz chirp.
- **Gameplay**: Variable jump height, multiple obstacles (cacti, birds), bonus multiplier, scrolling ground, clouds.
- **High Score**: Saved to EEPROM.

## Hardware Requirements

- Arduino Uno
- SSD1306 OLED (128x64, I2C)
- HC-SR04 Ultrasonic Sensor
- Passive Buzzer (2-pin)
- Jumper wires, breadboard

## Wiring

See schematic.md for details.

## Setup

1. **Install Libraries**:
   - Adafruit_GFX
   - Adafruit_SSD1306
   - EEPROM (built-in)
   - Install via Arduino IDE Library Manager.
2. **Upload Code**:
   - Open `src/DinoRunOLEDClassicDinoEnhanced.ino` in Arduino IDE.
   - Set Board to “Arduino Uno,” select port, upload.
3. **Play**:
   - Wave hand (&lt;15 cm) to start/jump/retry.
   - Hold hand for higher jumps.
   - Avoid obstacles to increase score and bonus multiplier.

## Files

- `src/DinoRunOLEDClassicDinoEnhanced.ino`: Main game code.
- `docs/schematic.md`: Wiring details.
- `docs/setup_guide.md`: Detailed setup instructions.

## Troubleshooting

- **No Display**: Check OLED address (0x3C) with I2C scanner. Adjust `dinoY` (e.g., 42) if misaligned.
- **No Sound**: Verify buzzer wiring. Test: `tone(5, 1000, 200);`.
- **No Jumps**: Ensure HC-SR04 is unobstructed. Debug distance via Serial Monitor (9600 baud).

## Customization

- **Dino Sprite**: Edit `dino1`, `dino2` bitmaps in code.
- **Sounds**: Modify `play*Sound` functions for new frequencies/durations.
- **Difficulty**: Adjust `obstacleSpeed`, `obstacleGap`, or spawn rate.

## License

MIT License

## Credits

Inspired by the Chrome Dino game. Built by Wired Wanderer.