#pragma weather_h

// averages pressure
unsigned long averPress(Adafruit_BMP280 *mybmp) {
  unsigned long pressure = 0, aver;
  for (byte i = 0; i < 10; ++i) {
    pressure += (*mybmp).readPressure();
  }
  return pressure / 10L;
}

// this class keeps data on current weather
class Weather {
  float temp, hum;
  unsigned long press;
  Timer timer;
  DHT_Unified *mydht;
  Adafruit_BMP280 *mybmp;
  bool changed;
public:
  Weather (unsigned long thr, DHT_Unified *dht, Adafruit_BMP280 *bmp) {
    timer.start(thr);
    mydht = dht;
    mybmp = bmp;
    changed = true;
  }
  void change() {
    changed = true;
  }
  void update() {
    if (timer.ready()) {
      timer.reset();
      getData();
    }
  }
  void getData() {
    press = averPress(mybmp);
    sensors_event_t event;
    (*mydht).temperature().getEvent(&event);
    temp = event.temperature;
    (*mydht).humidity().getEvent(&event);
    hum = event.relative_humidity;
    changed = true;
  }
  void print() {
    if (changed) {
      lcd.setCursor(0, 3);
      lcd.print((int)temp);
      lcd.print(F("."));
      lcd.print(((int)(temp * 10)) % 10);
      lcd.print((char)223);
      lcd.print(F("C"));

      lcd.setCursor(9, 3);
      lcd.print((int)hum);
      lcd.print(F("%"));

      String press_str = String((long)(press * 0.0075006)) + "mm";
      lcd.setCursor(20 - press_str.length(), 3);
      lcd.print(press_str);
      changed = false;
    }
  }
};

// updated once every 10 min, this guy keeps track of pressure
//    and tries to roughly forecast when the pressure has dropped enough
//    which means a probable rain within a few hours
class Forecast {
  Adafruit_BMP280 *mybmp;
  unsigned long pressure, pressure_array[6], time_array[6];
  Timer timer;
  int delta;
  bool changed;
public:
  Forecast(unsigned long thr, Adafruit_BMP280 *bmp) {
    timer.start(thr);
    mybmp = bmp;
    changed = true;
  }
  void init() {
    pressure = averPress(mybmp);
    for (byte i = 0; i < 6; ++i) {
      pressure_array[i] = pressure;
      time_array[i] = i;
    }
  }
  void change() {
    changed = true;
  }
  void update() {
    if (timer.ready()) {
      timer.reset();
      pressure = averPress(mybmp);
      for (byte i = 0; i < 5; ++i)
        pressure_array[i] = pressure_array[i + 1];
      pressure_array[5] = pressure;
      unsigned long sumX = 0, sumY = 0, sumX2 = 0, sumXY = 0;
      float a = 0;
      for (int i = 0; i < 6; i++) {
        sumX += time_array[i];
        sumY += (long)pressure_array[i];
        sumX2 += time_array[i] * time_array[i];
        sumXY += (long)time_array[i] * pressure_array[i];
      }
      a = (long)6 * sumXY;
      a = a - (long)sumX * sumY;
      a = (float)a / (6 * sumX2 - sumX * sumX);
      delta = a * 6;
      changed = true;
    }
  }
  void print() {
    if (changed) {
      delta = map(delta, -250, 0, 20, 1);
      delta = constrain(delta, 1, 20);
      lcd.setCursor(0, 2);
      for (byte i = 0; i < 20; ++i) {
        if (i < delta) lcd.print("-");
        else lcd.print(" ");
      }
      changed = false;
    }
  }
};
