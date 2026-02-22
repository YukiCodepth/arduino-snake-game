# arduino-snake-game
Real-time Arduino-based Snake Game demonstrating embedded system design, I/O handling, and hardware-software integration.

<p align="center">
  <img src="images/hardware_image.jpg" width="750">
</p>

<p align="center">
  <img src="images/working_video.gif" width="750">
</p>

# Arduino Snake Game (Hardware Implementation)

A real-time embedded Snake Game implemented on Arduino Uno using LCD display, push buttons, LEDs, potentiometer, and buzzer.  
This project demonstrates complete system-level design, hardware-software integration, and real-time control.

---

## Project Overview:

This project implements the classic Snake Game as a real-time embedded system application.  
It handles user inputs, timing control, display rendering, score processing, and output signaling using dedicated hardware components.

The main implementation is on physical hardware, with a separate Tinkercad simulation provided for reference and validation.

---

## Purpose of the Project:

- Demonstrate microcontroller-based real-time system design
- Apply embedded I/O handling and timing control concepts
- Integrate hardware and software into a complete system
- Provide a practical learning platform for embedded systems

---

## Features:

- Real-time snake movement
- LCD-based gameplay display
- Button-controlled navigation
- Speed control using potentiometer
- Binary score display using LEDs
- Game over indication with LED and buzzer
- Pause and resume functionality
- Multi-tone sound effects

---

## Components Used:

### Main Components
- Arduino Uno
- 16x2 LCD Display
- Push Buttons (3x)
- 10kΩ Potentiometer
- Buzzer
- Breadboard
- Jumper Wires

### LEDs and Resistors
- LEDs × 8
- 220Ω Resistors × 8

---

## Circuit Design:

The complete circuit was first designed and verified using Tinkercad and later implemented on physical hardware.

Detailed schematic and wiring diagrams are available in: /design/design.pdf



---

## Pin Configuration:

### LCD (Parallel Interface)
| Signal | Arduino Pin |
|--------|-------------|
| RS     | 7           |
| E      | 8           |
| D4-D7  | 9–12        |

### Buttons
| Function | Pin |
|----------|------|
| Start    | 2    |
| Up       | 3    |
| Down     | 4    |

### Outputs
| Component     | Pin  |
|---------------|------|
| Buzzer        | 13   |
| Score LEDs    | 5–8  |
| Game Over LED | 9    |
| Button LEDs   | 10–12|

### Potentiometer
| Terminal | Connection |
|----------|------------|
| Middle   | A0         |
| Others   | 5V, GND    |

---

## Project Structure:

arduino-snake-game/
│
├── hardware/ → Final hardware implementation
├── tinkercad/ → Simulation version
├── images/ → Photos and demo GIF
├── videos/ → Original demo video
├── design/ → Circuit documentation
└── README.md


---

## How to Run:

1. Connect all components as per schematic
2. Open Arduino IDE
3. Upload `hardware/snake_game_hardware.ino`
4. Power the board
5. Press Start to begin gameplay
6. Use Up and Down buttons to control movement

---

## Tinkercad Simulation:

A separate simulation version was developed in Tinkercad to verify circuit connections and logic before hardware deployment.

Location:/tinkercad


This version was used for:

- Circuit validation
- Logic testing
- Debugging
- Pin optimization

---

## Applications:

- Embedded systems education
- Microcontroller laboratory experiments
- Hardware-software co-design learning
- Technical exhibitions
- Portfolio development

---

## Author

Aman Kumar
Electronics and Communication Engineering  
Embedded Systems, IOT, lINUX, AI/ML 

GitHub: https://github.com/YukiCodepth

---

## License

This project is released for educational and personal use.

