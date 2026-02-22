#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD with address 0x27 (common address) and 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
const int startBtn = 2;    // START / PAUSE / RESUME (Toggle)
const int upBtn = 3;       // MOVE UP
const int downBtn = 4;     // MOVE DOWN
const int potPin = A0;     // Speed control potentiometer

// BCD LED pins for score display (4 bits for ones digit only)
const int ledBit0 = 5;     // (1)
const int ledBit1 = 6;     // (2)
const int ledBit2 = 7;     // (4)
const int ledBit3 = 8;     // (8) - Ones digit (1-2-4-8)

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

// Movement direction - Start moving right
int dirX = 1;
int dirY = 0;

// Food
int foodX, foodY;

// Timing
unsigned long lastMove = 0;

// Snake body arrays
int snakeBodyX[MAX_SNAKE_LENGTH];
int snakeBodyY[MAX_SNAKE_LENGTH];

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Set buttons to INPUT_PULLUP
  pinMode(startBtn, INPUT_PULLUP);
  pinMode(upBtn, INPUT_PULLUP);
  pinMode(downBtn, INPUT_PULLUP);

  // Set LED pins as OUTPUT
  pinMode(ledBit0, OUTPUT);
  pinMode(ledBit1, OUTPUT);
  pinMode(ledBit2, OUTPUT);
  pinMode(ledBit3, OUTPUT);

  Serial.println("Setup complete - Testing buttons...");
  
  randomSeed(analogRead(A5));
  resetGame();
  showWelcomeScreen();
}

void loop() {
  // SIMPLE BUTTON HANDLING
  if (digitalRead(startBtn) == LOW) {
    delay(50);
    if (digitalRead(startBtn) == LOW) {
      handleStartButton();
    }
  }

  if (digitalRead(upBtn) == LOW) {
    delay(50);
    if (digitalRead(upBtn) == LOW) {
      handleUpButton();
    }
  }

  if (digitalRead(downBtn) == LOW) {
    delay(50);
    if (digitalRead(downBtn) == LOW) {
      handleDownButton();
    }
  }

  // Game state management
  if (!gameRunning && !gameOver) {
    return;
  }

  if (!gameRunning && gameOver) {
    showGameOverScreen();
    delay(100);
    return;
  }

  // Game running logic
  int potValue = analogRead(potPin);
  int speedDelay = map(potValue, 0, 1023, 150, 400);

  if (millis() - lastMove >= speedDelay) {
    lastMove = millis();
    updateGame();
  }
}

void handleStartButton() {
  Serial.println("START button pressed");
  if (gameOver) {
    resetGame();
    gameRunning = true;
    Serial.println("Game reset and started");
  } else {
    gameRunning = !gameRunning;
    if (gameRunning) {
      lcd.clear();
      Serial.println("Game resumed");
    } else {
      showPauseScreen();
      Serial.println("Game paused");
    }
  }
  delay(300);
}

void handleUpButton() {
  Serial.println("UP button pressed");
  if (gameRunning) {
    // Only allow moving to top row when currently on bottom row and moving left
    if (snakeY == 1 && dirX == -1) {
      snakeY = 0;
      dirX = 1; // Change direction to right when moving to top
      Serial.println("Moved to top row, now moving right");
    }
  }
  delay(200);
}

void handleDownButton() {
  Serial.println("DOWN button pressed");
  if (gameRunning) {
    // Only allow moving to bottom row when currently on top row and moving right
    if (snakeY == 0 && dirX == 1) {
      snakeY = 1;
      dirX = -1; // Change direction to left when moving to bottom
      Serial.println("Moved to bottom row, now moving left");
    }
  }
  delay(200);
}

void updateGame() {
  // Update snake body
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeBodyX[i] = snakeBodyX[i - 1];
    snakeBodyY[i] = snakeBodyY[i - 1];
  }

  // Move head
  snakeX += dirX;
  snakeBodyX[0] = snakeX;
  snakeBodyY[0] = snakeY;

  // Wall collision - GAME OVER when hitting any wall
  if (snakeX < 0 || snakeX > 15 || snakeY < 0 || snakeY > 1) {
    gameOver = true;
    gameRunning = false;
    if (score > highScore) {
      highScore = score;
    }
    Serial.println("Game Over - Hit wall");
    return;
  }

  // Self-collision check
  if (checkSelfCollision()) {
    gameOver = true;
    gameRunning = false;
    if (score > highScore) {
      highScore = score;
    }
    Serial.println("Game Over - Self collision");
    return;
  }

  // Food collision
  if (snakeX == foodX && snakeY == foodY) {
    score++;
    snakeLength = min(snakeLength + 1, MAX_SNAKE_LENGTH);
    placeFood();
    Serial.println("Food eaten! Score: " + String(score));
    showEatEffect();
  }

  updateDisplay();
}

bool checkSelfCollision() {
  for (int i = 4; i < snakeLength; i++) {
    if (snakeBodyX[0] == snakeBodyX[i] && snakeBodyY[0] == snakeBodyY[i]) {
      return true;
    }
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
    
    // Avoid all corner positions (0,0), (15,0), (0,1), (15,1)
    if ((foodX == 0 && foodY == 0) || (foodX == 15 && foodY == 0) || 
        (foodX == 0 && foodY == 1) || (foodX == 15 && foodY == 1)) {
      validPosition = false;
      continue;
    }
    
    // Check if food overlaps with snake body
    for (int i = 0; i < snakeLength; i++) {
      if (foodX == snakeBodyX[i] && foodY == snakeBodyY[i]) {
        validPosition = false;
        break;
      }
    }
    
    attempts++;
    if (attempts > 200) {
      // Emergency placement - try all positions systematically except corners
      for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 2; y++) {
          // Skip all corner positions
          if ((x == 0 && y == 0) || (x == 15 && y == 0) || 
              (x == 0 && y == 1) || (x == 15 && y == 1)) {
            continue;
          }
          bool occupied = false;
          for (int i = 0; i < snakeLength; i++) {
            if (x == snakeBodyX[i] && y == snakeBodyY[i]) {
              occupied = true;
              break;
            }
          }
          if (!occupied) {
            foodX = x;
            foodY = y;
            Serial.println("Emergency food placement at: " + String(x) + "," + String(y));
            return;
          }
        }
      }
      // If all positions are occupied, place in any non-corner position
      do {
        foodX = random(0, 16);
        foodY = random(0, 2);
      } while ((foodX == 0 && foodY == 0) || (foodX == 15 && foodY == 0) || 
               (foodX == 0 && foodY == 1) || (foodX == 15 && foodY == 1));
      validPosition = true;
    }
  } while (!validPosition);
}

void updateDisplay() {
  lcd.clear();
  
  // Draw snake
  for (int i = 0; i < snakeLength; i++) {
    if (snakeBodyX[i] >= 0 && snakeBodyX[i] <= 15 && snakeBodyY[i] >= 0 && snakeBodyY[i] <= 1) {
      lcd.setCursor(snakeBodyX[i], snakeBodyY[i]);
      lcd.print(i == 0 ? "O" : "o");
    }
  }
  
  // Draw food
  lcd.setCursor(foodX, foodY);
  lcd.print("*");
  
  // Display score
  int scoreRow = (snakeY == 0) ? 1 : 0;
  lcd.setCursor(0, scoreRow);
  lcd.print("S:");
  lcd.print(score);
  
  // Update BCD LED display with 4 LEDs only
  updateBCDDisplay();
}

void updateBCDDisplay() {
  // Get ones digit from score (0-9)
  int ones = score % 10;
  
  // Ones digit: Pins 5-8 as 1-2-4-8
  digitalWrite(ledBit0, bitRead(ones, 0));   // Pin 5 = 1 (LSB)
  digitalWrite(ledBit1, bitRead(ones, 1));   // Pin 6 = 2
  digitalWrite(ledBit2, bitRead(ones, 2));   // Pin 7 = 4
  digitalWrite(ledBit3, bitRead(ones, 3));   // Pin 8 = 8 (MSB)
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
  // Clear LEDs on welcome screen
  clearLEDs();
}

void showPauseScreen() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("PAUSED");
  lcd.setCursor(1, 1);
  lcd.print("Press START");
  // Keep current score displayed on LEDs during pause
}

void showGameOverScreen() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("GAME OVER");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  // Keep final score displayed on LEDs
}

void resetGame() {
  snakeX = 8;
  snakeY = 0;
  snakeLength = 3;
  dirX = 1; // Start moving right
  score = 0;
  gameOver = false;
  
  // Initialize snake body
  for (int i = 0; i < MAX_SNAKE_LENGTH; i++) {
    snakeBodyX[i] = -1;
    snakeBodyY[i] = -1;
  }
  
  // Initial positions
  snakeBodyX[0] = 8; snakeBodyY[0] = 0;
  snakeBodyX[1] = 7; snakeBodyY[1] = 0;
  snakeBodyX[2] = 6; snakeBodyY[2] = 0;
  
  placeFood();
  lcd.clear();
  // Clear LEDs when game resets
  clearLEDs();
}

void clearLEDs() {
  digitalWrite(ledBit0, LOW);
  digitalWrite(ledBit1, LOW);
  digitalWrite(ledBit2, LOW);
  digitalWrite(ledBit3, LOW);
}