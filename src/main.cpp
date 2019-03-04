#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <DHT_U.h>
#include <GyverButton.h>

// Initialize all the hardware
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
Adafruit_BMP280 bmp;
DHT_Unified dht(DHT_PIN, DHT_TYPE);

GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

#include "aux.h"
#include "numbers.h"
#include "clock.h"
#include "date.h"
#include "weather.h"

// Initialize objects
Dots clock_dots(1000L);                                     // clock dots ":" (updated every 1 sec)
Date current_date(500L, &rtc);                              // to keep & update the current date (updated every 0.5 sec)
Weather current_weather(1000L, &dht, &bmp);                 // to keep & update the current temperature/humidity/pressure (updated every 1 sec)
Forecast current_forecast(1000L * 60L * 10L, &bmp);   // to compute rain probability from pressure dynamics (updated every 10 min)
BrightnessControl current_lcd_bright(100L);                 // to control LCD brightness (updated every 100 ms)

void respondToTap();

void setup() {
  // initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(LCD_BRIGHT_PIN, OUTPUT);

  // initialize DHT, BMP & RTC modules
  dht.begin();
  bmp.begin();
  if (! rtc.begin())
    while (1);
  if (!rtc.isrunning()) {
    // set date/time to system date/time + 30 seconds (compilation and upload time)
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(0, 0, 0, 30));
  }

  // adjust button parameters
  touch.setTimeout(300);
  touch.setStepTimeout(50);

  // initialize forecast
  current_forecast.init();
}

void loop () {
  // the loop is pretty self-descriptive
  touch.tick();
  respondToTap();

  current_date.update();
  current_date.print();
  clock_dots.update();

  current_weather.update();
  current_weather.print();

  current_forecast.update();
  current_forecast.print();

  current_lcd_bright.update();
}

void respondToTap() {
  // routine to analize and respond to taps
  if (touch.hasClicks()) {
    byte clicks = touch.getClicks();
    switch (clicks) {
      case 1:
        // I didn't want to do anything on single tap
        break;
      case 2:
        // turns off the LCD screen
        current_lcd_bright.changeState();
        break;
      case 3:
        // enter the date/time set mode
        clockAdjust(&rtc);
        lcd.clear();
        current_date.timer.reset();
        current_date.changed = true;
        current_forecast.change();
        current_weather.change();
        break;
      default:
        break;
    }
  }
}
