#include <Arduino.h>
#include <Wire.h>
#include <VescUart.h>
#include <Adafruit_SSD1306.h>

// OLED display globals 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// VESC globals
VescUart vesc;

int calculateBatteryPercentage(float voltage) {
  return 12.2 * (voltage - 45.5);
}

void setup() {
  Serial.begin(9600);

  // Setup VESC
  Serial2.begin(115200);
  while (!Serial2) {;}
  vesc.setSerialPort(&Serial2);

  // Setup OLED Display 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while(true) {
      Serial.println(F("SSD1306 allocation failed"));
    }
  }
  display.display();
  display.clearDisplay();  
  display.setRotation(2);
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setTextColor(WHITE);
}

void loop() {
  if ( vesc.getVescValues() ) {
    display.clearDisplay();
    display.setCursor(0,0);

    if (vesc.data.inpVoltage >= 54.6) {
      display.printf("OVERVOLT!\n");
    } else {
      int pct = calculateBatteryPercentage(vesc.data.inpVoltage);
      display.printf("BATT:%2d\n", pct);
    }

    display.printf("DEP:%.2fWh\n", vesc.data.wattHours);
    display.printf("CHR:%.2fWh\n", vesc.data.wattHoursCharged);
    if (vesc.data.avgMotorCurrent >= 0) {
      display.fillRect(0, SCREEN_HEIGHT * .75, int(3.2 * vesc.data.avgMotorCurrent), SCREEN_HEIGHT * .25, WHITE);
    } else {
      display.drawRect(0, SCREEN_HEIGHT * .75, abs(int(3.2 * vesc.data.avgMotorCurrent)), SCREEN_HEIGHT * .25, WHITE);
    }
    // if (vesc.data.inpVoltage > 54.6) {display.print("OVERVOLT!!!");}
  } else {
    display.clearDisplay();  
    display.setCursor(0,0);
    display.print("UART FAIL!");
  }

  display.display();
  delay(50);
}