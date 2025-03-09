# 🐍 Snake Game Arduino Mega and ESP32

This project is a two-part system that brings the classic Snake game to life using an **Arduino Mega** for game logic and an **ESP32** for web-based monitoring. The game is displayed on an 8x8 LED matrix and can be controlled using two buttons for left and right turns. The current game scene is also accessible via a web interface hosted by the ESP32.

## Components Used
- **ESP32**: Hosts a web server to display the game scene.
- **Arduino Mega**: Manages the game logic, button inputs, and LED matrix.
- **8x8 LED Matrix**: Displays the snake, food, and game scene.
- **Buttons**: Controls the snake's direction (left and right).

## Features
- **Wi-Fi Access Point**: ESP32 creates a SoftAP named `SSID_Sori` with the password `12345678`.
- **Web Interface**: 
  - Access game status at `http://192.168.4.1/` once connected.
  - Updates game scene every second.
- **Serial Communication**: ESP32 and Arduino Mega communicate via Serial2.
- **Dynamic Game Scene**: Snake moves, eats, and grows; game restarts on win or collision.
- **Real-time Feedback**: LED matrix reflects game status immediately.

### Hardware Connections
- **LED Matrix**:
  - DIN → Pin 50 (Mega)
  - CLK → Pin 51 (Mega)
  - CS  → Pin 53 (Mega)
- **Buttons**:
  - Left Button → Pin 2 (Mega)
  - Right Button → Pin 3 (Mega)
- **Serial Communication**:
  - Mega TX (Pin 17) → ESP32 RX (GPIO16)
  - Mega RX (Pin 16) → ESP32 TX (GPIO17)

![Screenshot 2025-01-13 170931](https://github.com/user-attachments/assets/15a3a81d-6e3f-442b-bc40-d13e0c7ed270)

### Controls
- **Left Button**: Turn the snake left.
- **Right Button**: Turn the snake right.

## Game Rules
- **Eat the Blob**: Increases snake length.
- **Win Condition**: Reach maximum length (`15`).
- **Lose Condition**: Collision with itself.
- **Score**: Displayed on Serial Monitor.

## Libraries Used
- **WiFi.h**: For ESP32 Wi-Fi functionality.
- **MaxMatrix.h**: For controlling the 8x8 LED matrix.

## Screenshot
- Simple Web interface showing the live game scene and score.
  
![WhatsApp Image 2025-03-09 at 12 02 17_f9cffc3f](https://github.com/user-attachments/assets/7842774c-d585-41fd-9a2d-5293af473f65)




