/*Biblioteci utilizate pentru afisarea graficului pe LCD*/
#include <SPI.h>
#include <TFT.h>

/*Biblioteca pentru a controla ledurile adresabile*/
#include <FastLED.h>

/*definim pinii necesare pentru comunicarea prin protocolul SPI.
   De remarcat faptul ca nu trebuie sa declaram pinul de clock (SCK sau SCL),
   si nici pinul de date (MOSI - master out slave it) deoarece vom utiliza
   pinii hardware prezenti pe placa arduino (SCK - 13, MOSI - 11).
   OBS: Deoarece LCD-ul este o componenta doar de iesire, pinul de date
   MISO (master in slave out) nu este utilizat.
*/
#define LCD_CS   10  //chip select
#define LCD_DC   9
#define LCD_RES  8
/*cream un nou obiect de tip TFT, prin intermediul caruia vom controla LCD-ul */
TFT screen = TFT(LCD_CS, LCD_DC, LCD_RES);

/*Buzzerul este conectat la pinul D6*/
#define BUZZER_PIN 6
#define BUZZER_TIME 1000

/*Inelul cu LED-uri este conectat la pinul D7*/
#define LED_PIN  7
/*Precizam cate LED-uri se afla pe inel*/
#define NUM_LEDS 16
#define LED_STEP 3
/*cream un obiect CRGB prin intermediul vom controla inelul de leduri*/
CRGB leds[NUM_LEDS];


/*Senzorul de alcool gazos este conectat la pinul A0*/
#define MQ3_PIN A0
/*Setam threshold-ul pentru care buzzerul se activeaza, dar si cand
   punctele de pe grafic devin rosii, deoarece s-a depasit pragul*/
#define THRESHOLD 25

void drawGraph() {
  /*setam cea mai mica dimensiune pentru font*/
  screen.setTextSize(1);
  /*setam culoarea alba pentru text si linii*/
  screen.stroke(255, 255, 255);

  /*originea*/
  screen.text("O", 4, 101);
  /*axa Y*/
  screen.line(10, 100, 10, 10);
  /*sagetile din capatul axei Y*/
  screen.line(10, 10, 7, 13);
  screen.line(10, 10, 13, 13);
  screen.text("[%]", 15, 10);

  /*gradatiile pentru axa Y*/
  for (int i = 19; i < 100; i = i + 9) {
    screen.line(8, i, 13, i);
  }


  /*axa X*/
  screen.line(10, 100, screen.width() - 20, 100);
  /*sagetile din capatul axei X*/
  screen.line(screen.width() - 20, 100, screen.width() - 23, 97);
  screen.line(screen.width() - 20, 100, screen.width() - 23, 103);
  screen.text("t",  screen.width() - 20, 105);
  /*gradatiile pentru axa X*/
  for (int i = 10 + 13; i < screen.width() - 20; i = i + 13) {
    screen.line(i, 98, i, 103);
  }


}

void setup() {
  Serial.begin(115200);
  /*initializam inelul cu leduri*/
  FastLED.addLeds < WS2812, LED_PIN, GRB > (leds, NUM_LEDS);

  /*configuram pin-ul utilizat de buzzer ca fiind iesire*/
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  /*initializam ecranul*/
  screen.begin();
  /*setam culoarea fundalului la negru*/
  screen.background(0, 0, 0);
  /*desenam graficul pe LCD*/
  drawGraph();
}

void triggerBuzzer(int percentage) {
  static long triggerTime = 0;

  if (percentage < THRESHOLD) {
    triggerTime = 0;
    digitalWrite(BUZZER_PIN, HIGH);
    return;
  }

  if (percentage > THRESHOLD && triggerTime == 0) {
    triggerTime = millis();
    digitalWrite(BUZZER_PIN, LOW);
    return;
  }

  long currentTime = millis();
  if (currentTime - triggerTime > BUZZER_TIME) {
    digitalWrite(BUZZER_PIN, HIGH);
  }

}

void displayValue(int percentage) {
  screen.setTextSize(2);
  screen.text("Value:", 10, 110);


  //transformam procentul din int in char
  char percentageValue[3];
  String(percentage).toCharArray(percentageValue, 3);

  /*desenam un dreptunghi negru peste valoarea anterioara*/
  screen.fill(0, 0, 0);
  screen.stroke(0, 0, 0);
  screen.rect(85, 108, 35, 20);

  /*daca procentul este peste nivelul setat, atunci valoarea pe ecran
     va fi scrisa cu rosu, si cu verde in caz contrar
  */
  if (percentage > THRESHOLD) {
    screen.stroke(0, 0, 255); //textul va avea culoarea rosie
  } else {
    screen.stroke(0, 255, 0); //textul va avea culoarea verde
  }
  screen.text(percentageValue, 90, 110);
  screen.stroke(255, 255, 255);
}

void ledRing(int percentage) {

  int maxValue = map(percentage, 0 , NUM_LEDS * LED_STEP, 0 , 15);
  int threshold = 8;
  for (int i = 0; i < 16; i++) {

    if (i == threshold) {
      leds[threshold] = CRGB(0, 0, 100);
    }
    if (maxValue > threshold) {
      leds[threshold] = CRGB(0, 100, 100);
    }
    if (i <= maxValue && i < threshold) {
      leds[i] = CRGB(0, 100, 0);
    }
    if (i <= maxValue && i > threshold) {
      leds[i] = CRGB(100, 0, 0);
    }
    if (i > maxValue && i != threshold) {
      leds[i] = CRGB(0, 0, 0);
    }
  }
  FastLED.show();
}

void sendDataToESP(int percentage) {
  static long lastTime = 0;
  long currentTime = millis();
  if (currentTime - lastTime > 200) {
    char sendChar = (char) percentage;
    Serial.write(sendChar);
  }
}

void addPointToGraph(int valueToAdd) {
  static int xContor = 10;
  xContor = xContor + 1;
  /*daca am depasit marginea din dreapta, resetam graficul*/
  if (xContor > screen.width() - 20) {
    screen.background(0, 0, 0);
    drawGraph();
    xContor = 10;
  }
  /*obtinem coordonata Y a noii valori*/
  int mappedValue = map(valueToAdd, 0, 1023, 100, 10);
  /*transformam valoarea analog citita in procente*/
  int percentage = map(valueToAdd, 0, 1023, 0, 100);

  if (percentage > THRESHOLD) {
    screen.stroke(0, 0, 255); //punctul va avea culoarea rosie
  } else {
    screen.stroke(0, 255, 0);//punctul va avea culoarea rosie
  }
  screen.point(xContor, mappedValue);
  screen.stroke(255, 255, 255); //punctul va avea culoarea alba

  displayValue(percentage);
  triggerBuzzer(percentage);
  ledRing(percentage);
  sendDataToESP(percentage);
}




void loop() {
  int currentValue = analogRead(A0);
  addPointToGraph(currentValue);
  delay(50);


}
