# smart-radar-hmi
Smart Radar &amp; Control Interface using Arduino, ultrasonic sensor, joystick, and TFT display.

A functional prototype of an embedded Human-Machine Interface (HMI) using an Arduino, an ultrasonic sensor, a joystick, and a TFT display. This project focuses on real-time data processing, UI rendering optimization, and hardware constraint management.

![Radar Demo](SR_V1.gif)

## 🚀 Key Engineering Features

### 1. Ultra-Fast Sensor Polling (Non-Blocking)
Standard ultrasonic sensor code often completely blocks the microcontroller. To keep the UI responsive, I implemented a strict hardware timeout:
`long duration = pulseIn(echo, HIGH, 6000);`
By capping the wait time to 6000 microseconds, the MCU avoids freezing when an object is out of range, ensuring the display loop maintains a high frame rate.

### 2. Debouncing & Multitasking
The system uses `millis()` to track time (`lastClickTime_J`, `lastClickTime_R`, and `FPS`), allowing the Arduino to handle joystick inputs, toggle system states, and control the relay simultaneously without using `delay()`.

### 3. Flicker-Free TFT Rendering
TFT displays driven by standard Arduino boards suffer from severe flickering if `tft.fillScreen()` is called inside the `loop()`. I optimized the rendering using two methods:
* **Trailing Whitespace Overwrite:** Instead of clearing the screen to update the distance value, I print spaces after the text (`tft.print("cm   ");`) to overwrite previous text ghosts.
* **Partial Bar Graph Refresh:** The visual distance bar is drawn in two parts. First, the active color bar (Red/Green) is drawn, immediately followed by a white rectangle that dynamically erases the exact remainder of the previous bar length:
  ```cpp
  tft.fillRect(0, 70, barWidth, 10, barColor);
  tft.fillRect(barWidth, 70, 128 - barWidth, 10, ST77XX_WHITE);


🛠️ Dealing with Cloned Hardware

The project utilizes a low-cost clone of the ST7735 display. When attempting to use standard commands, the display exhibited inverted colors and static artifacts.
Calling tft.invertDisplay(true); corrected the BGR/RGB color channel issue, but introduced a persistent static effect. Instead of abandoning the hardware, I adapted the UI design to remain readable and functional despite the static interference, demonstrating resilience and problem-solving with imperfect hardware.

🔌 Hardware Pinout

| Component | Pin Connection |
| :--- | :--- |
| **HC-SR04 Trigger** | D5 |
| **HC-SR04 Echo** | D6 |
| **TFT CS / RST** | D4 / D12 |
| **TFT A0 (DC) / SDA / SCL** | D3 / D11 / D13 |
| **Relay** | D8 |
| **Joystick X / Y / Btn**| A0 / A1 / D2 |
