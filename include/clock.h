#pragma clock_h

// these guys just draw the clock + dots

void drawClock(byte hours, byte minutes, byte x, byte y) {
  lcd.setCursor(x, y);
  lcd.print("               ");
  lcd.setCursor(x, y + 1);
  lcd.print("               ");

  if (hours / 10 == 0) drawDig(10, x, y);
  else drawDig(hours / 10, x, y);
  drawDig(hours % 10, x + 4, y);
  drawDig(minutes / 10, x + 8, y);
  drawDig(minutes % 10, x + 12, y);
}

void drawDots(byte x, byte y, boolean state) {
  byte code;
  if (state) code = 165;
  else code = 32;
  lcd.setCursor(x, y);
  lcd.write(code);
  lcd.setCursor(x, y + 1);
  lcd.write(code);
}

class Dots {
  bool cond;
  Timer timer;
public:
  Dots (unsigned long thr) {
    timer.start(thr);
    cond = true;
  }
  void update() {
    if (timer.ready()) {
      drawDots(7, 0, cond);
      timer.reset();
      cond = !cond;
    }
  }
};
