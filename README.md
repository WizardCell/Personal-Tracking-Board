# Personal Tracking Board - An RGB matrix panel that helps users track their day to day tasks.

## Project by: Ronie Odeh, Khalid Dabbah, and Mohammmad Mossa.

Personal tracking board, an Internet of Things (IOT) based project that helps track the progress of user-created assignments per board user. This is done with the help of: ESP32, HUB75 LED matrix panel, push button, power button, Flutter app, Firestore and Firebase databases.

---

# Folder Structure of the Repository
```
├── Documentation - wiring diagram, state diagram, instructions, etc.
├── ESP - source code for the esp side (firmware).
├── FlutterApp - dart code for our Flutter app.
├── Unit Tests - tests for individual hardware components (input / output devices)
├── PARAMETERS.md - hard coded parameters of the project.
└── README.md - this file.
```

# Libraries used for ESP32 and their versions
- [ESP32 Firebase](https://github.com/Rupakpoddar/ESP32Firebase) - 1.0.0
- [ArduinoJson](https://arduinojson.org/) - 7.0.4
- [WiFiManger](https://github.com/tzapu/WiFiManager) - 2.0.17

# Hardware used
- 1 $\times$ ESP32
- 1 $\times$ HUB75 RGB LED Panel
- 1 $\times$ Push Button
- 1 $\times$ Power Button

# Wiring Diagram
![Wiring Diagram](/Documentation/WiringDiagram.jpg)

# Poster
![Project Poster](/Documentation/poster.jpg)

This project is part of ICST - The Interdisciplinary Center for Smart Technologies, Taub Faculty of Computer Science, Technion
https://icst.cs.technion.ac.il/
