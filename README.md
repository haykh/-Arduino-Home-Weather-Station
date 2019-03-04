# Home Weather Station firmware

Home Weather Station project on Arduino Nano (code for [PlatformIO][1]).

All the settings and definitions can be adjusted in `platformio.ini` file.

### Few things to note
1. Make sure to adjust the potentiometer on I2C.
2. The firmware prints days of the week in Russian, for English simply change the `daysOfTheWeek` array (line 24 `date.h`) and remove all the `if`-s in `printDayOfWeek()` function.
3. Brightness control of the LCD relies on the photoresistor analog output (voltage), and may require recalibration for your environment. Please consider recalibrating it in line 34 `aux.h`.
4. The library included in `main.cpp` is for BMP280. For BME280 use a different one and adjust the object name in line 15 in `main.cpp` and everywhere else.
5. Same for RTC module (see line 13 of `main.cpp`).

Below is the schematics of this whole story.

![scheme](https://github.com/haykh/-Arduino-Home-Weather-Station/blob/master/scheme.png)

### Things used:
- Arduino Nano
- RTC DS1307 (or any other RTC): keep time when turned off
- TTP 223: touch sensor
- BMP280 (or BME280): pressure
- DHT11: temperature & humidity
- LCD 2004a (with I2C)
- 5V DC power adapter
- Photoresistor: to control the brightness of LCD depending on background light
- 10 kOhm resistor

[1]: https://docs.platformio.org/en/latest/ide/atom.html
