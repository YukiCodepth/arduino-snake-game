# Arduino Snake Game 🐍
**Real-time Arduino-based Snake Game demonstrating embedded system design, I/O handling, and hardware-software integration.**

<p align="center">
  <img src="images/hardware_image.jpeg" width="750">
</p>

## 📍 Navigation
[Features](#features) • [Components](#components-used) • [Pin Config](#pin-configuration) • [Tinkercad](#tinkercad-simulation) • [Project Structure](#project-structure)

---

## Project Overview
This project implements the classic Snake Game as a real-time embedded system application. It handles user inputs, timing control, display rendering, score processing, and output signaling using dedicated hardware components.

The main implementation is on physical hardware, with a separate Tinkercad simulation provided for reference and validation.

<p align="center">
  <img src="images/working_sample.gif" width="750">
</p>

## Purpose of the Project
- **Real-Time Design:** Demonstrate microcontroller-based timing control concepts.
- **I/O Handling:** Apply embedded peripheral management.
- **Integration:** Combine hardware and software into a functional, consumer-ready system.
- **Education:** Provide a practical portfolio project for Embedded Systems and ECE.

---

## Features
- ✅ **Real-time movement:** Smooth snake navigation.
- ✅ **LCD Display:** 16x2 character interface for gameplay.
- ✅ **Tactile Control:** 3-button navigation (Start, Up, Down).
- ✅ **Dynamic Speed:** Real-time difficulty adjustment using a potentiometer.
- ✅ **Binary Score:** 8-LED array displaying score in binary format.
- ✅ **Audio/Visual Alerts:** Buzzer tones and "Game Over" LED signals.
- ✅ **Pause/Resume:** Ability to freeze the game state.

---

## Components Used

### Main Components
- **Microcontroller:** Arduino Uno
- **Display:** 16x2 LCD Display
- **Inputs:** Push Buttons (3x), 10kΩ Potentiometer
- **Feedback:** Passive Buzzer, Breadboard, Jumper Wires

### LEDs and Resistors
- **Score Display:** 8x LEDs
- **Protection:** 8x 220Ω Resistors

---

## Pin Configuration

### LCD (Parallel Interface)
| Signal | Arduino Pin |
|--------|-------------|
| RS     | 7           |
| E      | 8           |
| D4-D7  | 9–12        |

### Buttons
| Function | Pin |
|----------|-----|
| Start    | 2   |
| Up       | 3   |
| Down     | 4   |

### Outputs
| Component     | Pin  |
|---------------|------|
| Buzzer        | 13   |
| Score LEDs    | 5–8  |
| Game Over LED | A1* |
| Status LEDs   | A2* |
> *Note: Using Analog pins as Digital outputs to prevent conflicts with LCD pins 9-12.*

---

## Project Structure

arduino-snake-game/
│
├── hardware/   → Final hardware implementation (.ino)
├── tinkercad/  → Simulation version and links
├── images/     → Photos and demo GIF
├── videos/     → Original high-res demo video
├── design/     → Circuit schematics and PDF documentation
└── README.md   → Project documentation

---

## How to Run

1. **Hardware Setup:** Connect all components according to the `design/design.pdf` schematic.
2. **Software:** Open the Arduino IDE.
3. **Upload:** Flash `hardware/snake_game_hardware.ino` to your Arduino Uno.
4. **Play:** Power the board, press **Start**, and use the **Up/Down** buttons to play!

---

## Tinkercad Simulation

A separate simulation version was developed in Tinkercad to verify circuit connections and logic before hardware deployment. Use this to test the logic without physical components.

**Location:** /tinkercad

---

## Author

**Aman Kumar** *Electronics and Communication Engineering* Focus: Embedded Systems | IoT | Linux | AI/ML

[GitHub Profile](https://github.com/YukiCodepth)

---

## License

This project is released for educational and personal use.
