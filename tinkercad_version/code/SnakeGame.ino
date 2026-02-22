#include <LiquidCrystal.h>

// Initialize the LCD with direct pin connections (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Pin definitions
const int startBtn = 2;    // START / PAUSE / RESUME (Toggle)
const int upBtn = 3;       // MOVE UP
const int downBtn = 4;     // MOVE DOWN
const int potPin = A0;     // Speed control potentiometer
const int buzzerPin = 13;  // Buzzer pin for all sounds

// BCD LED pins for score display (4 bits for ones digit only)
const int ledBit0 = 5;     // (1)
const int ledBit1 = 6;     // (2)
const int ledBit2 = 7;     // (4)
const int ledBit3 = 8;     // (8) - Ones digit (1-2-4-8)

// Game Over LED
const int gameOverLed = 9; // Game Over indicator LED

// Button LEDs
const int startBtnLed = 10; // LED for START button
const int upBtnLed = 11;    // LED for UP button  
const int downBtnLed = 12;  // LED for DOWN button

// Game constants
const int MAX_SNAKE_LENGTH = 32;

// Snake properties
int snakeX = 8;
int snakeY = 0;
int snakeLength = 3;
int score = 0;
int highScore = 0;
bool gameRunning = false;
bool gameOver = false;
bool hitWall = false; // Track if game over was due to wall hit
bool startSoundPlayed = false; // Track if start sound has been played

// Movement direction - Start moving right
int dirX = 1;
int dirY = 0;

// Food
int foodX, foodY;

// Timing
unsigned long lastMove = 0;
unsigned long lastBlink = 0;
unsigned long lastFade = 0;
unsigned long lastLineEffect = 0;
bool ledState = false;
int fadeBrightness = 255;
int fadeDirection = -5;
int linePosition = 0;

// Button timing
unsigned long buttonPressTime = 0;
bool buttonHandled = false;
bool upButtonHandled = false;
bool downButtonHandled = false;

// Snake body arrays
int snakeBodyX[MAX_SNAKE_LENGTH];
int snakeBodyY[MAX_SNAKE_LENGTH];

void setup() {
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  lcd.clear();

  // Button setup
  pinMode(startBtn, INPUT_PULLUP);
  pinMode(upBtn, INPUT_PULLUP);
  pinMode(downBtn, INPUT_PULLUP);

  // LED pins setup
  pinMode(ledBit0, OUTPUT);
  pinMode(ledBit1, OUTPUT);
  pinMode(ledBit2, OUTPUT);
  pinMode(ledBit3, OUTPUT);
  pinMode(gameOverLed, OUTPUT);

  // Button indicator LEDs
  pinMode(startBtnLed, OUTPUT);
  pinMode(upBtnLed, OUTPUT);
  pinMode(downBtnLed, OUTPUT);

  // Buzzer pin
  pinMode(buzzerPin, OUTPUT);

  // Initialize all LEDs to OFF
  clearAllLEDs();

  randomSeed(analogRead(A5));
  resetGame();
  showWelcomeScreen();
}

// === LOOP ===
void loop() {
  // Handle button inputs with LED feedback
  handleButtons();
  
  if (!gameRunning && !gameOver) return;

  if (!gameRunning && gameOver && hitWall) {
    if (millis() - lastBlink >= 100) {
      lastBlink = millis();
      ledState = !ledState;
      digitalWrite(gameOverLed, ledState ? HIGH : LOW);
    }
    showGameOverScreen();
    updateLEDDisplay(); // Show score on LEDs after game over
    delay(50);
    return;
  }

  if (!gameRunning && gameOver && !hitWall) {
    if (millis() - lastFade >= 30) {
      lastFade = millis();
      fadeBrightness += fadeDirection;
      if (fadeBrightness <= 0) {
        fadeBrightness = 0;
        fadeDirection = 5;
      } else if (fadeBrightness >= 255) {
        fadeBrightness = 255;
        fadeDirection = -5;
      }
      analogWrite(gameOverLed, fadeBrightness);
    }
    showGameOverScreen();
    updateLEDDisplay(); // Show score on LEDs after game over
    delay(50);
    return;
  }

  int potValue = analogRead(potPin);
  int speedDelay = map(potValue, 0, 1023, 150, 400);

  if (millis() - lastMove >= speedDelay) {
    lastMove = millis();
    updateGame();
  }
}

// === BUTTON HANDLER ===
void handleButtons() {
  // START Button
  if (digitalRead(startBtn) == LOW) {
    if (!buttonHandled) {
      digitalWrite(startBtnLed, HIGH);
      buttonPressTime = millis();
      buttonHandled = true;
      handleStartButton();
    }
  } else {
    if (buttonHandled && millis() - buttonPressTime > 50) {
      digitalWrite(startBtnLed, LOW);
      buttonHandled = false;
    }
  }

  // UP Button - FIXED: Only glow when pressed
  if (digitalRead(upBtn) == LOW) {
    if (!upButtonHandled) {
      digitalWrite(upBtnLed, HIGH);
      upButtonHandled = true;
      if (gameRunning) {
        handleUpButton();
      }
    }
  } else {
    if (upButtonHandled) {
      digitalWrite(upBtnLed, LOW);
      upButtonHandled = false;
    }
  }

  // DOWN Button - FIXED: Only glow when pressed
  if (digitalRead(downBtn) == LOW) {
    if (!downButtonHandled) {
      digitalWrite(downBtnLed, HIGH);
      downButtonHandled = true;
      if (gameRunning) {
        handleDownButton();
      }
    }
  } else {
    if (downButtonHandled) {
      digitalWrite(downBtnLed, LOW);
      downButtonHandled = false;
    }
  }
}

// === BUTTON HANDLERS ===
void handleStartButton() {
  if (gameOver) {
    resetGame();
    gameRunning = true;
    playStartGameSound();
  } else {
    gameRunning = !gameRunning;
    if (gameRunning) {
      lcd.clear();
      if (!startSoundPlayed) {
        playStartGameSound();
        startSoundPlayed = true;
      }
    } else {
      showPauseScreen();
    }
  }
  delay(300);
}

void handleUpButton() {
  if (gameRunning) {
    if (snakeY == 1 && dirX == -1) {
      snakeY = 0;
      dirX = 1;
    }
  }
}

void handleDownButton() {
  if (gameRunning) {
    if (snakeY == 0 && dirX == 1) {
      snakeY = 1;
      dirX = -1;
    }
  }
}

// === GAME LOGIC ===
void updateGame() {
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeBodyX[i] = snakeBodyX[i - 1];
    snakeBodyY[i] = snakeBodyY[i - 1];
  }

  snakeX += dirX;
  snakeBodyX[0] = snakeX;
  snakeBodyY[0] = snakeY;

  if (snakeX < 0 || snakeX > 15 || snakeY < 0 || snakeY > 1) {
    gameOver = true;
    gameRunning = false;
    hitWall = true;
    if (score > highScore) highScore = score;
    digitalWrite(gameOverLed, HIGH);
    playGameOverSound();
    return;
  }

  if (checkSelfCollision()) {
    gameOver = true;
    gameRunning = false;
    hitWall = false;
    if (score > highScore) highScore = score;
    playGameOverSound();
    return;
  }

  if (snakeX == foodX && snakeY == foodY) {
    score++;
    snakeLength = min(snakeLength + 1, MAX_SNAKE_LENGTH);
    placeFood();
    playFoodSound();
    showEatEffect();
  }

  updateDisplay();
}

bool checkSelfCollision() {
  for (int i = 4; i < snakeLength; i++) {
    if (snakeBodyX[0] == snakeBodyX[i] && snakeBodyY[0] == snakeBodyY[i]) return true;
  }
  return false;
}

void placeFood() {
  bool validPosition;
  int attempts = 0;
  
  do {
    validPosition = true;
    foodX = random(0, 16);
    foodY = random(0, 2);

    if ((foodX == 0 && foodY == 0) || (foodX == 15 && foodY == 0) ||
        (foodX == 0 && foodY == 1) || (foodX == 15 && foodY == 1)) validPosition = false;

    for (int i = 0; i < snakeLength; i++) {
      if (foodX == snakeBodyX[i] && foodY == snakeBodyY[i]) {
        validPosition = false;
        break;
      }
    }

    attempts++;
    if (attempts > 200) validPosition = true;
  } while (!validPosition);
}

void updateDisplay() {
  lcd.clear();
  for (int i = 0; i < snakeLength; i++) {
    if (snakeBodyX[i] >= 0 && snakeBodyX[i] <= 15 && snakeBodyY[i] >= 0 && snakeBodyY[i] <= 1) {
      lcd.setCursor(snakeBodyX[i], snakeBodyY[i]);
      lcd.print(i == 0 ? "O" : "o");
    }
  }
  lcd.setCursor(foodX, foodY);
  lcd.print("*");
  
  // Score LEDs stay OFF during gameplay - no LED update here
}

void updateLEDDisplay() {
  // Only show score in binary after game over
  int displayValue = score;
  digitalWrite(ledBit0, (displayValue & 1) ? HIGH : LOW);    // Bit 0 (1)
  digitalWrite(ledBit1, (displayValue & 2) ? HIGH : LOW);    // Bit 1 (2)
  digitalWrite(ledBit2, (displayValue & 4) ? HIGH : LOW);    // Bit 2 (4)
  digitalWrite(ledBit3, (displayValue & 8) ? HIGH : LOW);    // Bit 3 (8)
}

void showEatEffect() {
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("YUM!");
  lcd.setCursor(4, 1);
  lcd.print("+1 PT!");
  delay(300);
  lcd.clear();
}

void showWelcomeScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("== SNAKE GAME ==");
  lcd.setCursor(0, 1);
  lcd.print("Press START");
  clearAllLEDs();
}

void showPauseScreen() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("PAUSED");
  lcd.setCursor(1, 1);
  lcd.print("Press START");
}

void showGameOverScreen() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("GAME OVER");
  lcd.setCursor(0, 1);
  lcd.print(hitWall ? "Hit Wall! " : "Self Hit! ");
  lcd.print(score);
}

void resetGame() {
  snakeX = 8; snakeY = 0;
  snakeLength = 3; dirX = 1;
  score = 0; gameOver = false;
  hitWall = false; ledState = false;
  fadeBrightness = 255; fadeDirection = -5;
  linePosition = 0; startSoundPlayed = false;

  for (int i = 0; i < MAX_SNAKE_LENGTH; i++) {
    snakeBodyX[i] = -1;
    snakeBodyY[i] = -1;
  }

  snakeBodyX[0] = 8; snakeBodyY[0] = 0;
  snakeBodyX[1] = 7; snakeBodyY[1] = 0;
  snakeBodyX[2] = 6; snakeBodyY[2] = 0;
  
  placeFood();
  lcd.clear();
  clearAllLEDs();
  digitalWrite(gameOverLed, LOW);
  noTone(buzzerPin);
}

void playFoodSound() { tone(buzzerPin, 1000, 100); }
void playStartGameSound() {
  tone(buzzerPin, 800, 80); delay(100);
  tone(buzzerPin, 600, 80); delay(100);
  tone(buzzerPin, 1000, 100);
}
void playGameOverSound() {
  tone(buzzerPin, 600, 100); delay(120);
  tone(buzzerPin, 400, 100); delay(120);
  tone(buzzerPin, 300, 150);
}

void clearLEDs() {
  digitalWrite(ledBit0, LOW);
  digitalWrite(ledBit1, LOW);
  digitalWrite(ledBit2, LOW);
  digitalWrite(ledBit3, LOW);
}

void clearAllLEDs() {
  clearLEDs();
  digitalWrite(startBtnLed, LOW);
  digitalWrite(upBtnLed, LOW);
  digitalWrite(downBtnLed, LOW);
  digitalWrite(gameOverLed, LOW);
}