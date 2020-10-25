# arduino-ethylotest
A project I did for my Microcontroller course in university.
It's essentially an ethylotest using Arduino.
Used the following parts:
  MQ-3 Alcohol Gas sensor
  Arduino Uno + ESP 8266
  Buzzer module
  16 LED Ring RGB WS2812
  LCD Module 1.8" ST7735

The project has the following functionalities:
  Reading values from the sensor 
  Showing the alcohol level using LEDs
  Using a buzzer if the value is too high
  Writing a measuring scale and showing the measured value on the LCD (using the TFT and SPI libraries)
  Showing all the values in a mobile app (using the Blynk app)
  
The simple .ino file is for the first four, the -esp.ino file is to connect with the Blynk app, also using the ESP8266WiFi and BlynkSimpleEsp8266 libraries.
