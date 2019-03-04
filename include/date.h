#pragma date_h

// custom Russian letters
uint8_t P_[8] = {0b11111,  0b10001,  0b10001,  0b10001,  0b10001,  0b10001,  0b10001,  0b00000};
uint8_t Ch_[8] = {0b10001,  0b10001,  0b10001,  0b01001,  0b00111,  0b00001,  0b00001,  0b00000};

// maximum day of a given month
int maxday(int mon, int yr) {
  if ((mon == 2) && (yr % 100 == 0) && (yr % 400 != 0)) return 28;
  if ((mon == 2) && (yr % 4 == 0)) return 29;
  if (mon == 2) return 28;
  if ((mon < 8) && (mon % 2 != 0)) return 31;
  if ((mon >= 8) && (mon % 2 == 0)) return 31;
  return 30;
}

void loadLetters() {
  lcd.createChar(0, P_);
  lcd.createChar(5, Ch_);
}

// yeah, this is Russian
//    Just put here whatever you wish in English
String daysOfTheWeek[7] = {"BC", "11", "BT", "CP", "44", "55", "C6"};

void printDate(int day, int mon, byte x, byte y) {
  lcd.setCursor(x, y);
  lcd.print(day);
  lcd.print("/");
  if (mon < 10) lcd.print(0);
  lcd.print(mon);
}

// yeah, this is Russian
//    Just put here whatever you wish in English
void printDayOfWeek(String weekday, byte x, byte y) {
  loadLetters();
  lcd.setCursor(x, y);
  if (weekday == "11") {
    lcd.write(0);
    lcd.print("H");
  } else if (weekday == "55") {
    lcd.write(0);
    lcd.print("T");
  } else if (weekday == "44") {
    lcd.write(5);
    lcd.print("T");
  } else {
    // for English, remove all the `if`-s and leave only this
    //    after adjusting the `daysOfTheWeek` array above
    lcd.print(weekday);
  }
}

// to keep the current date
class Date {
public:
  int year, month, day;
  String weekday;
  int hour, min;
  Timer timer;
  RTC_DS1307 *myrtc;
  bool changed;
  Date (unsigned long thr, RTC_DS1307 *mrtc) {
    timer.start(thr);
    myrtc = mrtc;
    changed = true;
  }
  void update() {
    if (timer.ready()) {
      timer.reset();
      getTime();
    }
  }
  void getTime() {
    year = (*myrtc).now().year();
    month = (*myrtc).now().month();
    day = (*myrtc).now().day();
    weekday = daysOfTheWeek[(*myrtc).now().dayOfTheWeek()];
    hour = (*myrtc).now().hour();
    if (min != (*myrtc).now().minute()) {
      changed = true;
      min = (*myrtc).now().minute();
    }
  }
  void print() {
    if (changed) {
      drawClock(hour, min, 0, 0);
      printDate(day, month, 15, 0);
      printDayOfWeek(weekday, 17, 1);
      changed = false;
    }
  }
};

// maybe this can be done easier, but I just hardcoded the clock set routine using this class (function itself is further down)
class DateAdjustable {
  int year_, month_, day_, hour_, min_, sec_;
  byte mode;
  Timer timer;
  boolean tick;
public:
  DateAdjustable (int yy, int mm, int dd, int hh, int min, int sec, unsigned long thr) : year_(yy), month_(mm), day_(dd), hour_(hh), min_(min), sec_(sec) {
    timer.start(thr);
    mode = 0;
  }
  void adjustUp() {
    switch(mode) {
    case 0:
      hour_ = (hour_ + 1) % 24;
      drawHour();
      break;
    case 1:
      min_ = (min_ + 1) % 60;
      drawMin();
      break;
    case 2:
      day_ = day_ + 1;
      if (day_ > maxday(month_, year_)) day_ = 1;
      printDay();
      break;
    case 3:
      month_ = month_ + 1;
      if (month_ > 12) month_ = 1;
      printMon();
      break;
    case 4:
      year_ = (year_ - 2018 + 1) % 32 + 2018;
      printYear();
      break;
    default:
      break;
    }
  }
  void adjustDown() {
    switch(mode) {
    case 0:
      hour_ = hour_ - 1;
      if (hour_ < 0) hour_ = 23;
      drawHour();
      break;
    case 1:
      min_ = min_ - 1;
      if (min_ < 0) min_ = 59;
      drawMin();
      break;
    case 2:
      day_ = day_ - 1;
      if (day_ < 1) day_ = maxday(month_, year_);
      printDay();
      break;
    case 3:
      month_ = month_ - 1;
      if (month_ < 1) month_ = 12;
      printMon();
      break;
    case 4:
      year_ = (year_ - 2018 - 1) % 32 + 2018;
      printYear();
      break;
    default:
      break;
    }
  }
  void switchMode() {
    mode = (mode + 1) % 5;
    printInit(false);
    tick = true;
    timer.reset();
  }
  void printInit(bool again) {
    if (again) {
      drawHour();
      drawMin();
    }
    lcd.setCursor(0, 2);
    lcd.print("               ");
    printDay();
    lcd.print("/");
    printMon();
    printYear();
    drawDots(7, 0, true);
    tick = true;
  }
  void printTick() {
    if (timer.ready()) {
      timer.reset();
      switch(mode) {
      case 0:
        lcd.setCursor(0, 2);
        lcd.print("-------");
        break;
      case 1:
        lcd.setCursor(8, 2);
        lcd.print("-------");
        break;
      case 2:
        if (!tick) {
          lcd.setCursor(15, 0);
          lcd.print("  ");
        } else {
          printDay();
        }
        tick = !tick;
        break;
      case 3:
        if (!tick) {
          lcd.setCursor(18, 0);
          lcd.print("  ");
        } else {
          printMon();
        }
        tick = !tick;
        break;
      case 4:
        if (!tick) {
          lcd.setCursor(16, 1);
          lcd.print("    ");
        } else {
          printYear();
        }
        tick = !tick;
        break;
      default:
        break;
      }
    }
  }
  void drawHour() {
    lcd.setCursor(0, 0);
    lcd.print("       ");
    lcd.setCursor(0, 1);
    lcd.print("       ");
    if (hour_ / 10 == 0) drawDig(10, 0, 0);
    else drawDig(hour_ / 10, 0, 0);
    drawDig(hour_ % 10, 4, 0);
  }
  void drawMin() {
    lcd.setCursor(8, 0);
    lcd.print("       ");
    lcd.setCursor(8, 1);
    lcd.print("       ");
    drawDig(min_ / 10, 8, 0);
    drawDig(min_ % 10, 12, 0);
  }
  void printDay() {
    lcd.setCursor(15, 0);
    if (day_ < 10) lcd.print(" ");
    lcd.print(day_);
  }
  void printMon() {
    lcd.setCursor(18, 0);
    if (month_ < 10) lcd.print(0);
    lcd.print(month_);
  }
  void printYear() {
    lcd.setCursor(16, 1);
    lcd.print(year_);
  }
  void modeDelay() {
    unsigned long delay_time;
    switch(mode) {
    case 0:
      delay_time = 500L;
      break;
    case 1:
      delay_time = 300L;
      break;
    case 2:
      delay_time = 500L;
      break;
    case 3:
      delay_time = 500L;
      break;
    case 4:
      delay_time = 500L;
      break;
    default:
      break;
    }
    delay(delay_time);
  }
  void addTime(unsigned long time_pass) {
    DateTime dt0 (year_, month_, day_, hour_, min_, sec_);
    dt0 = dt0 + TimeSpan(0, 0, 0, time_pass / 1000L);
    year_ = dt0.year();
    month_ = dt0.month();
    day_ = dt0.day();
    hour_ = dt0.hour();
    min_ = dt0.minute();
    sec_ = dt0.second();
  }
  void saveTime(RTC_DS1307 *myrtc) {
    (*myrtc).adjust(DateTime(year_, month_, day_, hour_, min_, sec_));
  }
};

// to set the clock manually using the touch sensor
//    in short:
//      hold    = increase value (day ++, hour ++ etc)
//      1 tap   = decrease value
//      2 taps  = change mode (hour -> min -> day -> month -> year)
//      3 taps  = exit mode
//    it also adds the time spent on setting the time
void clockAdjust(RTC_DS1307 *myrtc) {
  unsigned long time_passed = millis();
  bool date_changed = false, wasStep = false;
  Timer wait_time;
  lcd.clear();

  // save the old values
  int year_, month_, day_, hour_, min_, sec_;
  year_ = (*myrtc).now().year(); month_ = (*myrtc).now().month(); day_ = (*myrtc).now().day();
  hour_ = (*myrtc).now().hour(); min_ = (*myrtc).now().minute(); sec_ = (*myrtc).now().second();

  DateAdjustable date_(year_, month_, day_, hour_, min_, sec_, 500L);
  date_.printInit(true);

  wait_time.start(20000L);
  while(!wait_time.ready()) {
    if (touch.hasClicks()) {
      byte taps = touch.getClicks();
      if (taps != 0) {
        wait_time.reset();
        if (taps == 2) date_.switchMode();
        if (taps == 1) {
          date_.adjustDown();
          date_changed = true;
        }
        if (taps == 3) break;
      }
    } else if (touch.isStep()) {
      wait_time.reset();
      date_.adjustUp();
      date_.modeDelay();
      wasStep = true;
      date_changed = true;
    } else if (touch.isRelease() && wasStep) {
      wasStep = false;
    } else {
      date_.printTick();
    }
    touch.tick();
  }
  if (date_changed) {
    date_.addTime(millis() - time_passed);
    date_.saveTime(myrtc);
  }
}
