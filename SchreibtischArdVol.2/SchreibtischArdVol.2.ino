#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float Luftfeuchtigkeit;
float Temperatur;
//-----------------------
const int pResistor = A3;
int Licht;
//-------------------------
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

int colorR = 0;
int colorG = 0;
int colorB = 0;
//--------------------------
const int Motor = 10;
const int Lampe = 13;
long Zeit;
long vergangeneZeit;
int LiOTemp = 1;
//-----------------------


void setup() {

  Serial.begin(9600);

  dht.begin();

  lcd.begin(16, 2);
  // lcd.setRGB(colorR, colorG, colorB);

  pinMode(pResistor, INPUT);
  pinMode(Motor, OUTPUT);
  delay(2000); //Zwei Sekunden Vorlaufzeit bis zur Messung (der Sensor ist etwas träge)
}



void loop() {

  Luftfeuchtigkeit = dht.readHumidity();
  Temperatur = dht.readTemperature();
  // Temperatur = 40;
  Licht = analogRead(pResistor);

  Zeit = millis();

  if (vergangeneZeit < Zeit) {
    vergangeneZeit = Zeit + 5000;

    if (LiOTemp == 2) {
      LiOTemp = 1;
      lcd.clear();
    } else {
      LiOTemp = 2;
      lcd.clear();
    }
  }

  Bildschirm();
  //debug();
  Ventilator();
  Flasche();
}



void Bildschirm() {

  switch (LiOTemp) {

    case 1:
      // lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Luftfeu: ");
      lcd.print(Luftfeuchtigkeit);
      lcd.setCursor(0, 1);
      lcd.print("Temp: ");
      lcd.setCursor(9, 1);
      lcd.print(Temperatur);
      break;

    case 2:
      //  lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Licht: ");
      lcd.print(Licht);
      break;
  }
}


void debug() {
  Serial.print("Luftfeuchtigkeit: ");
  Serial.println(Luftfeuchtigkeit);
  Serial.print("Temperatur: ");
  Serial.println(Temperatur);

  Serial.print("Licht: ");
  Serial.println(Licht);
}

void Ventilator() {
  if (Temperatur > 26) {
    digitalWrite(Motor, HIGH);
  } else {
    digitalWrite(Motor, LOW);
  }
}

void  Flasche() {
  if (Licht < 150) {
    digitalWrite(Lampe, HIGH);
    Serial.println("Lampe an");
  } else {
    digitalWrite(Lampe, LOW);
    
    Serial.println("Lampe aus");
  }
}
