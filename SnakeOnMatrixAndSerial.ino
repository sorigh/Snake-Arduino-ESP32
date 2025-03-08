// Program for a 2-button snake game printing to Serial monitor.

#include <SPI.h>
#include "Arduino.h"
#include "MaxMatrix.h"


// Buttons used for controlling the snake
#define left_button 2
#define right_button 3

#define LEDMATRIX_PIN_DIN	50
#define LEDMATRIX_PIN_CLK	51
#define LEDMATRIX_PIN_CS	53


volatile byte move_left = 0;
volatile byte move_right = 0;

// Variables for snake
int snake_l = 2;
const int max_len = 15;
int snake[max_len][2];
byte cur_heading = 0;

// Variable for food blob
int blob[2] = { 0, 0 };
int is_eaten = 1;

// The game scene
byte scene[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


MaxMatrix ledMatrix(LEDMATRIX_PIN_DIN,LEDMATRIX_PIN_CS,LEDMATRIX_PIN_CLK);
int ledMatrixtextScrollingSpeed = 80; 
char welcomeMessage[] = "Welcome to snake!";
char winMessage[] = "You Win!";
char loseMessage[] = "Game Over! Collision detected.";

// Function to initialize the game variables
void init_game() {
  is_eaten = 1;
  move_left = 0;
  move_right = 0;
  cur_heading = 0;
  snake_l = 2;
  for (int i = 0; i < max_len; i++)
    for (int j = 0; j < 2; j++)
      snake[i][j] = 0;
  snake[max_len - 1][0] = 2;
  snake[max_len - 1][1] = 5;
  snake[max_len - 2][0] = 1;
  snake[max_len - 2][1] = 5;
  refresh_scene();
  while ((move_left || move_right) == 0)
    ;
  move_left = 0;
  move_right = 0;
}

// Function to draw snake on the game scene
void spawn_snake() {
  for (int j = max_len - snake_l; j < max_len; j++) {
    if (snake[j][0] <= 0)
      snake[j][0] = 8 + snake[j][0];
    else if (snake[j][0] >= 9)
      snake[j][0] = snake[j][0] - 8;
    if (snake[j][1] <= 0)
      snake[j][1] = 8 + snake[j][1];
    else if (snake[j][1] >= 9)
      snake[j][1] = snake[j][1] - 8;

    scene[snake[j][0] - 1] |= (1 << (snake[j][1] - 1));
  }
}

// Function to update the position and length of the snake
void snake_move() {
  if (snake[max_len - 1][0] == blob[0] && snake[max_len - 1][1] == blob[1]) {
    is_eaten = 1;
    snake_l += 1;
  }

  for (int i = snake_l - 1; i >= 1; i--) {
    snake[max_len - 1 - i][0] = snake[max_len - i][0];
    snake[max_len - 1 - i][1] = snake[max_len - i][1];
  }

  if (move_left == 1) {
    if (cur_heading == 0) {
      cur_heading = 1;
      snake[max_len - 1][1] -= 1;
    } else if (cur_heading == 1) {
      cur_heading = 2;
      snake[max_len - 1][0] -= 1;
    } else if (cur_heading == 2) {
      cur_heading = 3;
      snake[max_len - 1][1] += 1;
    } else if (cur_heading == 3) {
      cur_heading = 0;
      snake[max_len - 1][0] += 1;
    }
    move_left = 0;
  } else if (move_right == 1) {
    if (cur_heading == 0) {
      cur_heading = 3;
      snake[max_len - 1][1] += 1;
    } else if (cur_heading == 1) {
      cur_heading = 0;
      snake[max_len - 1][0] += 1;
    } else if (cur_heading == 2) {
      cur_heading = 1;
      snake[max_len - 1][1] -= 1;
    } else if (cur_heading == 3) {
      cur_heading = 2;
      snake[max_len - 1][0] -= 1;
    }
    move_right = 0;
  } else {
    if (cur_heading == 0) {
      snake[max_len - 1][0] += 1;
    } else if (cur_heading == 1) {
      snake[max_len - 1][1] -= 1;
    } else if (cur_heading == 2) {
      snake[max_len - 1][0] -= 1;
    } else if (cur_heading == 3) {
      snake[max_len - 1][1] += 1;
    }
  }
}

// Function to generate a blob and draw the blob on the game scene
void blob_generator() {
  if (is_eaten) {
    blob[0] = random(1, 9);
    blob[1] = random(1, 9);
  }
  scene[blob[0] - 1] |= (1 << (blob[1] - 1));
  is_eaten = 0;
}

// Function to redraw the game scene and print to Serial
void refresh_scene() {
  for (int i = 0; i < 8; i++)
    scene[i] = 0x00;
  snake_move();
  spawn_snake();
  blob_generator();
  Serial.println("Game Scene:");
  for (int i = 0; i < 8; i++) {
    for (int j = 7; j >= 0; j--) {
      bool isOn = scene[i] & (1 << j); // Check if the bit is set
      ledMatrix.setDot(j, i, isOn); 
      Serial.print((scene[i] & (1 << j)) ? "#" : ".");

    }
    Serial.println();
  }
  Serial.println();
  Serial.print("Score:");
  Serial.println(snake_l);
  Serial.println("!");
}

// Callback for interrupt attached to left button
void update_left() {
  move_left = 1;
}

// Callback for interrupt attached to right button
void update_right() {
  move_right = 1;
}
// Setup function
void setup() {
  Serial.begin(9600);
  pinMode(left_button, INPUT_PULLUP);
  pinMode(right_button, INPUT_PULLUP);

  ledMatrix.init(1);      // Initialize with 1 matrix
  ledMatrix.setIntensity(3);
  ledMatrix.printStringWithShift(welcomeMessage, ledMatrixtextScrollingSpeed);


  randomSeed(analogRead(0));

  attachInterrupt(digitalPinToInterrupt(left_button), update_left, FALLING);
  attachInterrupt(digitalPinToInterrupt(right_button), update_right, FALLING);

  sei();
  init_game();
}

void loop() {
  if (snake_l == max_len) {
    Serial.println(winMessage);
    ledMatrix.printStringWithShift(winMessage, ledMatrixtextScrollingSpeed);
    delay(5000);
    init_game ();
  }

  for (int i = 0; i < max_len - 1; i++) {
    if (snake[i][0] == snake[max_len - 1][0] && snake[i][1] == snake[max_len - 1][1]) {
      ledMatrix.printStringWithShift(loseMessage, ledMatrixtextScrollingSpeed);
      Serial.println(loseMessage);
      delay(1000);
      init_game();
      break;
    }
  }

  refresh_scene();
  delay(1000);
}
