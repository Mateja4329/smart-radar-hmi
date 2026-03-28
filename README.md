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
