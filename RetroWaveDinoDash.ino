#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// OLED Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins
#define TRIG 9
#define ECHO 10
#define BUZZER 5

// Dino variables
float dinoY = 44; // Adjusted for 16x16 sprite
int dinoHeight = 16;
bool isJumping = false;
float jumpSpeed = 0;
const float gravity = 1.0; // Increased for faster descent
bool dinoLeg = false;

// Dino sprites (16x16)
const unsigned char dino1[] PROGMEM = {
  0x00, 0x00, 0x00, 0x3C, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0xFE, 0x00, 0xFE,
  0x00, 0xFE, 0x07, 0xFE, 0x3F, 0xFC, 0x3F, 0xF8, 0x1F, 0xF0, 0x0F, 0xE0,
  0x07, 0xE0, 0x03, 0xC0, 0x01, 0xE0, 0x00, 0xE0
};
const unsigned char dino2[] PROGMEM = {
  0x00, 0x00, 0x00, 0x3C, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0xFE, 0x00, 0xFE,
  0x00, 0xFE, 0x07, 0xFE, 0x3F, 0xFC, 0x3F, 0xF8, 0x1F, 0xF0, 0x0F, 0xE0,
  0x07, 0xE0, 0x03, 0x80, 0x01, 0xC0, 0x01, 0xC0
};

// Obstacle variables
struct Obstacle {
  int x;
  int type; // 0: small cactus, 1: tall cactus, 2: bird low, 3: bird mid, 4: bird high, 5: double cactus
  int y;
  bool active;
};
Obstacle obstacles[3];

// Game variables
bool inMenu = true;
bool gameOver = false;
int score = 0;
int highScore = 0;
unsigned long lastObstacleMove = 0;
int obstacleSpeed = 3;
const int maxObstacleSpeed = 8;
int obstacleGap = 80;
int bonusMultiplier = 1;
int consecutiveAvoids = 0;
unsigned long lastSpawnTime = 0;

// Visuals
int cloudX = 100;
int cloudY = 10;
int groundOffset = 0;

// Buzzer control
bool buzzerOn = false;
unsigned long buzzerStart = 0;
unsigned long buzzerDuration = 0;
int soundIndex = 0;
struct Sound { int freq; unsigned long dur; };
Sound currentSequence[4];
int sequenceLength = 0;

void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);

  highScore = EEPROM.read(0) * 256 + EEPROM.read(1);
  randomSeed(analogRead(A1));
}

void loop() {
  // Non-blocking buzzer sequence
  if (buzzerOn && millis() - buzzerStart >= buzzerDuration) {
    noTone(BUZZER);
    soundIndex++;
    if (soundIndex < sequenceLength) {
      tone(BUZZER, currentSequence[soundIndex].freq, currentSequence[soundIndex].dur);
      buzzerStart = millis();
      buzzerDuration = currentSequence[soundIndex].dur;
    } else {
      buzzerOn = false;
      soundIndex = 0;
      sequenceLength = 0;
    }
  }

  if (inMenu) {
    menuLoop();
  } else {
    if (!gameOver) {
      playGame();
    } else {
      showGameOver();
    }
  }
}

int getDistance() {
  long sum = 0;
  int validCounts = 0;
  for (int i = 0; i < 3; i++) {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    long duration = pulseIn(ECHO, HIGH, 30000);
    int dist = duration * 0.034 / 2;
    if (dist > 0 && dist < 200) {
      sum += dist;
      validCounts++;
    }
    delayMicroseconds(50);
  }
  return (validCounts > 0) ? sum / validCounts : -1;
}

void playSoundSequence(Sound seq[], int len) {
  if (!buzzerOn) {
    for (int i = 0; i < len; i++) currentSequence[i] = seq[i];
    sequenceLength = len;
    soundIndex = 0;
    tone(BUZZER, currentSequence[0].freq, currentSequence[0].dur);
    buzzerOn = true;
    buzzerStart = millis();
    buzzerDuration = currentSequence[0].dur;
  }
}

void playJumpSound() {
  Sound seq[] = {{900, 30}, {1100, 30}, {1300, 30}};
  playSoundSequence(seq, 3);
}

void playLevelUpSound() {
  Sound seq[] = {{1200, 50}, {1400, 50}, {1600, 50}, {1800, 50}};
  playSoundSequence(seq, 4);
}

void playGameOverSound() {
  Sound seq[] = {{600, 100}, {450, 100}, {300, 100}, {150, 100}};
  playSoundSequence(seq, 4);
}

void playMenuSound() {
  Sound seq[] = {{1200, 80}, {1500, 80}};
  playSoundSequence(seq, 2);
}

void playStepSound(bool leg) {
  Sound seq[] = {{leg ? 600 : 800, 40}};
  playSoundSequence(seq, 1);
}

void menuLoop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20);
  display.println("DINO RUN");
  display.setCursor(10, 40);
  display.println("Wave hand to Start");
  display.drawBitmap(10, 44, dino1, 16, 16, SSD1306_WHITE);
  display.drawCircle(cloudX, cloudY, 5, SSD1306_WHITE);
  display.drawCircle(cloudX + 8, cloudY, 4, SSD1306_WHITE);
  display.display();

  int distance = getDistance();
  if (distance > 0 && distance < 15) {
    playMenuSound();
    delay(500);
    inMenu = false;
    resetGame();
  }
}

void playGame() {
  int distance = getDistance();

  // Jump control (fixed jump)
  static unsigned long lastJumpTrigger = 0;
  if (!isJumping && distance > 0 && distance < 15 && millis() - lastJumpTrigger > 200) {
    isJumping = true;
    jumpSpeed = 10; // Fixed initial speed
    playJumpSound();
    lastJumpTrigger = millis();
  }

  // Jump physics
  if (isJumping) {
    dinoY -= jumpSpeed;
    jumpSpeed -= gravity;
    if (dinoY >= 44) {
      dinoY = 44;
      isJumping = false;
      jumpSpeed = 0;
    }
    if (dinoY < 0) dinoY = 0;
  }

  // Walking sound and animation
  static unsigned long lastStep = 0;
  if (!isJumping && millis() - lastStep > 300) {
    dinoLeg = !dinoLeg;
    playStepSound(dinoLeg);
    lastStep = millis();
  }

  // Dynamic difficulty
  obstacleSpeed = 3 + score / 100;
  if (obstacleSpeed > maxObstacleSpeed) obstacleSpeed = maxObstacleSpeed;
  obstacleGap = 80 - score / 40;
  if (obstacleGap < 40) obstacleGap = 40;

  // Move obstacles
  if (millis() - lastObstacleMove > 30) {
    for (int i = 0; i < 3; i++) {
      if (obstacles[i].active) {
        obstacles[i].x -= obstacleSpeed;
        if (obstacles[i].x < -20) {
          obstacles[i].active = false;
          score += bonusMultiplier;
          consecutiveAvoids++;
          if (consecutiveAvoids % 5 == 0) {
            bonusMultiplier++;
            playLevelUpSound();
          }
        }
      }
    }
    lastObstacleMove = millis();
  }

  // Spawn obstacles
  if (millis() - lastSpawnTime > obstacleGap * 10 && random(100) < 5) {
    for (int i = 0; i < 3; i++) {
      if (!obstacles[i].active) {
        obstacles[i].x = 128;
        obstacles[i].type = random(0, 6);
        if (obstacles[i].type >= 2 && obstacles[i].type <= 4) {
          obstacles[i].y = (obstacles[i].type - 2) * 10 + 10;
        }
        obstacles[i].active = true;
        lastSpawnTime = millis();
        break;
      }
    }
  }

  // Collision detection
  int dinoLeft = 10, dinoRight = 26, dinoTop = (int)dinoY, dinoBottom = (int)dinoY + dinoHeight;
  for (int i = 0; i < 3; i++) {
    if (obstacles[i].active) {
      int obsLeft = obstacles[i].x, obsRight = obsLeft + (obstacles[i].type == 5 ? 20 : obstacles[i].type >= 2 ? 12 : 10);
      int obsTop, obsBottom;
      if (obstacles[i].type == 0) {
        obsTop = 50;
        obsBottom = 60;
      } else if (obstacles[i].type == 1) {
        obsTop = 40;
        obsBottom = 60;
      } else if (obstacles[i].type == 5) {
        obsTop = 50;
        obsBottom = 60;
      } else {
        obsTop = obstacles[i].y;
        obsBottom = obsTop + 8;
      }

      if (dinoRight > obsLeft && dinoLeft < obsRight && dinoBottom > obsTop && dinoTop < obsBottom) {
        gameOver = true;
        playGameOverSound();
        consecutiveAvoids = 0;
        bonusMultiplier = 1;
        if (score > highScore) {
          highScore = score;
          EEPROM.update(0, highScore % 256);
          EEPROM.update(1, highScore / 256);
        }
        break;
      }
    }
  }

  // Draw everything
  display.clearDisplay();

  // Scrolling ground
  display.drawLine(0, 60, 128, 60, SSD1306_WHITE);
  display.drawLine(groundOffset, 60, groundOffset + 10, 60, SSD1306_WHITE);
  display.drawLine(groundOffset + 20, 60, groundOffset + 30, 60, SSD1306_WHITE);
  groundOffset = (groundOffset - obstacleSpeed) % 40;

  // Clouds
  cloudX = (cloudX - 1) % 128;
  if (cloudX < -12) cloudX = 128;
  display.drawCircle(cloudX, cloudY, 5, SSD1306_WHITE);
  display.drawCircle(cloudX + 8, cloudY, 4, SSD1306_WHITE);

  // Dino
  display.drawBitmap(10, (int)dinoY, isJumping ? dino1 : (dinoLeg ? dino1 : dino2), 16, 16, SSD1306_WHITE);

  // Jump indicator
  if (isJumping) {
    int jumpBarHeight = map((int)dinoY, 0, 44, 20, 0);
    display.fillRect(120, 60 - jumpBarHeight, 4, jumpBarHeight, SSD1306_WHITE);
  }

  // Obstacles
  for (int i = 0; i < 3; i++) {
    if (obstacles[i].active) {
      if (obstacles[i].type == 0) {
        display.fillRect(obstacles[i].x, 50, 10, 10, SSD1306_WHITE);
      } else if (obstacles[i].type == 1) {
        display.fillRect(obstacles[i].x, 40, 10, 20, SSD1306_WHITE);
      } else if (obstacles[i].type == 5) {
        display.fillRect(obstacles[i].x, 50, 20, 10, SSD1306_WHITE);
      } else {
        display.fillRect(obstacles[i].x, obstacles[i].y, 12, 8, SSD1306_WHITE);
      }
    }
  }

  // Score and stats
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(70, 0);
  display.print("Score: ");
  display.print(score);
  display.setCursor(70, 10);
  display.print("High: ");
  display.print(highScore);
  display.setCursor(70, 20);
  display.print("Bonus: ");
  display.print(bonusMultiplier);
  display.print("x");

  display.display();
}

void showGameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 20);
  display.println("GAME OVER");
  display.setCursor(10, 40);
  display.println("Wave hand to Retry");
  display.drawBitmap(10, 44, dino1, 16, 16, SSD1306_WHITE);
  display.display();

  int distance = getDistance();
  if (distance > 0 && distance < 15) {
    playMenuSound();
    delay(500);
    resetGame();
  }
}

void resetGame() {
  dinoY = 44;
  isJumping = false;
  jumpSpeed = 0;
  for (int i = 0; i < 3; i++) {
    obstacles[i].x = 128;
    obstacles[i].active = false;
  }
  score = 0;
  obstacleSpeed = 3;
  obstacleGap = 80;
  bonusMultiplier = 1;
  consecutiveAvoids = 0;
  gameOver = false;
  lastSpawnTime = millis();
  lastObstacleMove = millis();
}