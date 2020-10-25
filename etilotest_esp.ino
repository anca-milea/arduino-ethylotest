/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "zA9tOJZzLNCy08RpAkX2CZVxvo3Ae0W8";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "DIGI-4d95";
char pass[] = "2pJhKaH3";

SimpleTimer timer;
#define percentageChannel V0

void uploadData(int valueToSend) {
  Blynk.virtualWrite(percentageChannel, valueToSend);
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  /*send data to the server every 50 ms*/

}

void loop()
{
  Blynk.run();
  //valoarea convertita
  static int valueToSend = 0;
  static long lastTime = 0;

  //calculam timpul pentru a nu trimite constant valoarea, ci la 200ms
  long currentTime = millis();
  if (currentTime - lastTime > 200) {
    //trimitem datele
    uploadData(valueToSend);
    lastTime = millis();
  }

  if (Serial.available()) {
    char c;
    while (Serial.available()) {
      c = Serial.read();
    }
    valueToSend = (int) c;
  }
}
