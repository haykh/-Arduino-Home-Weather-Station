#pragma aux_h

// `delay()` is a bad habit,
//    so instead I use this `Timer` class to keep track of how much time has passed
class Timer {
  unsigned long current;
  unsigned long threshold;
public:
  Timer () : current(millis()) {}
  void start(unsigned long thr) {
    threshold = thr;
    reset();
  }
  void reset() {
    current = millis();
  }
  bool ready() {
    return (millis() - current) > threshold;
  }
};

// this guy takes care of LCD brightness reading analog from photoresistor & PWN-ing further
class BrightnessControl {
  Timer timer;
  bool lcd_on;
public:
  BrightnessControl(unsigned long thr) {
    timer.start(thr);
    lcd_on = true;
  }
  void update() {
    if (timer.ready() && lcd_on) {
      timer.reset();
      analogWrite(LCD_BRIGHT_PIN, constrain(map(analogRead(PHOT_PIN), 240, 1024, 100, 255), 100, 255));
      // ^ read photoresistor voltage & set lcd brightness using PWN
    }
  }
  void changeState() {
    lcd_on = !lcd_on; // turn LCD on/off
    if (!lcd_on) analogWrite(LCD_BRIGHT_PIN, 0);
  }
};
