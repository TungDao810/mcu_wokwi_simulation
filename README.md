# 🛠️ Microcontroller Simulations – Wokwi Projects

This repository contains simulation projects developed for the Microcontroller course using Arduino Mega, simulated in [Wokwi](https://wokwi.com/). Topics include timers, LCD interfacing, and external interrupts.

---

## 🔗 Live Simulations

| Project                    | Description                                   | Live Link |
|----------------------------|-----------------------------------------------|-----------|
| 🕒 **Timer Led blinking**  | Uses Timer1 to toggle LED every 1 sec         | [Run on Wokwi](https://wokwi.com/projects/404632875260108801) |
| 📺 **LCD Display**         | Displays text on 16x2 LCD via GPIO            | [Run on Wokwi](https://wokwi.com/projects/404826481610498049) |
| ⏱️ **Stopwatch**           | Start/stop/reset with interrupts              | [Run on Wokwi](https://wokwi.com/projects/401546243170751489) |

---

## 📁 Project Folders

Each folder contains:
- `main.ino` is main program 
- **Timer Led blinking**: Blinks LED using Timer1 in CTC mode: 0.5s ON, 2s OFF via compare match
- **LCD display**: Initializes and controls a 16x2 LCD in 4-bit mode using GPIO. Supports writing strings, integers, and floating-point values at specified cursor positions. Demonstrates scrolling text with custom messages.
- **Stopwatch**: Displays real-time stopwatch on 16x2 LCD with 0.1s precision using Timer1 (CTC mode). Controlled via push-button interrupt (INT1).


---

## 🧰 Tools Used
- **Arduino Mega**
- **Wokwi Simulation Environment**
- **16x2 LCD**, Timers, External Interrupts
- Language: **C/C++**

---

## 📝 Notes
All simulations were done as part of university coursework to practice embedded system concepts. Projects demonstrate skills in timers, peripheral control, and interrupt-driven programming.

