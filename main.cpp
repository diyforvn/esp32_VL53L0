#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "Adafruit_VL53L0X.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

bool blinkState = false;     // Trạng thái nhấp nháy
unsigned long lastBlink = 0; // Thời gian lần cuối đổi trạng thái
int blinkInterval = 200;     // Tốc độ nhấp nháy (ms)

void setup() {
  Serial.begin(115200);
  Wire.begin();


  if (!display.begin(0x3c, true)) {
    Serial.println("OLED init failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);

  if (!lox.begin()) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println(F("Khong thay VL53L0X"));
    display.display();
    while (1);
  }
  delay(1000);
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); // đo khoảng cách

  //display.clearDisplay();
  display.fillRect(0, 0, 128, 48, SH110X_BLACK); // chỉ xóa phần trên (text), không xóa vùng bar

  if (measure.RangeStatus != 4) { // nếu đo hợp lệ
    int distance = measure.RangeMilliMeter;

     // --- Hiển thị tiêu đề ---
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println(F("VL53L0X"));

    // --- Hiển thị số đo ---
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(15, 18);
    display.print(distance);
    display.println(F("mm"));

    // --- vẽ thanh đo ---
    int barLength = map(distance, 0, 2000, 0, 120); // max 2m
    if (barLength > 120) barLength = 120;
    display.drawRect(4, 50, 120, 10, SH110X_WHITE);

    // nếu khoảng cách nhỏ hơn 100mm thì nhấp nháy
    if (distance < 100) {
      if (millis() - lastBlink > blinkInterval) {
        blinkState = !blinkState;
        lastBlink = millis();
      }

      // Xóa vùng bar trước khi vẽ mới
      display.fillRect(5, 51, 118, 8, SH110X_BLACK);
      if (blinkState) {
        display.fillRect(4, 50, barLength, 10, SH110X_WHITE);
      }
    } else {
      display.fillRect(5, 51, 118, 8, SH110X_BLACK);
      display.fillRect(4, 50, barLength, 10, SH110X_WHITE);
    }

    Serial.print("Khoang cach: ");
    Serial.print(distance);
    Serial.println(" mm");
  } 
  else {
    display.fillRect(0, 0, 128, 48, SH110X_BLACK);
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.println(F("Out"));
  }

  display.display();
  delay(100);
}
