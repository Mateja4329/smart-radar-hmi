#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>


// --- TFT Display ---
const int TFT_CS   = 4;
const int TFT_RST  = 12;
const int TFT_A0   = 3;   // DC pin
const int TFT_SDA  = 11;  // MOSI
const int TFT_SCL  = 13;  // SCK
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_A0, TFT_SDA, TFT_SCL, TFT_RST);

// --- LED ---
const int pin_d1 = 7;

// --- Joystick ---
const int pin_x = A0;
const int pin_y = A1;
const int button = 2;

int prevXAng = 0;
int prevYAng = 0;

long lastClickTime_J = 0;

// --- Relay ---
const int relayPin = 8;
unsigned long lastClickTime_R = 0;

// --- Ultrasound ---
const int trig = 5;
const int echo = 6;

// --- globals ---
int old_button_state = 1;
int systemON = 0;
int distance = 0;
unsigned long prevDist = 0;
unsigned long FPS = 0;
const unsigned long cooldown = 1000;
bool isMoving = false;


void setup() {
  // tft display
  tft.initR(INITR_BLACKTAB);
  tft.invertDisplay(true); // this makes the "static" effect appear on the display... -_-
  tft.fillScreen(ST77XX_WHITE);

  tft.setCursor(5, 50);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.print("System OFF");

  // led
  pinMode(pin_d1, OUTPUT);
  digitalWrite(pin_d1, LOW);

  // joystick
  pinMode(button, INPUT_PULLUP);

  // relay
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  // ultrasound
  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  pinMode(echo, INPUT);

  Serial.begin(9600);
}


void loop() {
  int xVal = analogRead(pin_x);
  int yVal = analogRead(pin_y);
  int xAng = map(xVal, 0, 1023, 0, 180) - 4;
  int yAng = map(yVal, 0, 1023, 0, 180) - 1;

  xAng = constrain(xAng, 0, 180);
  yAng = constrain(yAng, 0, 180);
  int buttonState = digitalRead(button);
  
  // --- SYSTEM ON/OFF ---
  if(buttonState == 0 && old_button_state != buttonState && cooldown && (millis() - lastClickTime_J) >= cooldown)
  {
    systemON = !systemON;
    tft.fillScreen(ST77XX_WHITE);

    if (!systemON) {
      tft.setCursor(5, 50);
      tft.setTextColor(ST77XX_RED);
      tft.print("System OFF");
    }
    lastClickTime_J = millis();
  }

  old_button_state = buttonState;
  // --- SENSOR LOGIC ---
  if(systemON)
  {
    if((millis() - FPS) >= 200)
    {
      // 1. Measure
      digitalWrite(trig, LOW);
      delayMicroseconds(2);
      digitalWrite(trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig, LOW);

      long duration = pulseIn(echo, HIGH, 6000);
      int raw_distance = (duration / 2) * 0.034;

      // --- FILTER FOR STABILITY ---
      // Taking values between 2 and 400
      if(raw_distance > 2 && raw_distance < 400) {
        distance = raw_distance;
      }
      else {
        distance = 999;
      }

      if(distance != prevDist) {
        Serial.print("distance: ");
        Serial.print(distance);
        Serial.println("cm");

        // --- 1. STATUS ---
        tft.setCursor(5, 10);
        tft.setTextColor(ST77XX_GREEN, ST77XX_WHITE);
        tft.print("System ON   "); 

        // --- DISTANCE ---
        tft.setCursor(0, 40);
        tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
        tft.print("Dist: ");
        
        if (distance == 999) {
          tft.print("---cm   "); // extra spacing will delete the "text ghosts"
        } else {
          tft.print(distance);
          tft.print("cm   ");
        }
        
        // --- MINI RADAR (BAR GRAPH) ---
        int barWidth = map(distance, 2, 100, 0, 128); 
        barWidth = constrain(barWidth, 0, 128);
        
        // choosing color by distance
        uint16_t barColor = ST77XX_GREEN; // default color
        if(distance <= 50) {
          barColor = ST77XX_RED; // if its too close
        }

        // draw a line
        tft.fillRect(0, 70, barWidth, 10, barColor);
        
        // draws a white rectangle to delete the part of the previous
        tft.fillRect(barWidth, 70, 128 - barWidth, 10, ST77XX_WHITE);

        // save distance for the next loop
        prevDist = distance;
      }

      // 3. Relay control
      if(distance > 0 && distance <= 50 && (millis() - lastClickTime_R) >= cooldown)
      {
        digitalWrite(relayPin, HIGH);
        lastClickTime_R = millis();
      }
      else if(distance >= 55) {
        digitalWrite(relayPin, LOW);
      }
      FPS = millis();
    }
  }
  else {
    digitalWrite(relayPin, LOW);
  }

  if(abs(xAng - prevXAng) > 1 || abs(yAng - prevYAng) > 1 || old_button_state != buttonState)
  {
    digitalWrite(pin_d1, HIGH);

    Serial.print("X: ");
    Serial.print(xAng);
    Serial.print("X: ");
    Serial.print(" | Y: ");
    Serial.print(yAng);
    Serial.print(" | button: ");
    Serial.println(systemON);
  }

  digitalWrite(pin_d1, LOW);
  prevXAng = xAng;
  prevYAng = yAng;
} 
