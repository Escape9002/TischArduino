#include "DHT.h"
#define DHTPIN 12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float Luftfeuchtigkeit;
float Temperatur;
//-----------------------
const int pResistor = A0;
int Licht;
//-------------------------
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;
//--------------------------
const int Motor = 10;
const int Lampe = 8;
long Zeit;
long vergangeZeit;
//-----------------------


void setup() {

  Serial.begin(9600);

  dht.begin();

  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);

  pinMode(pResistor, INPUT);
  pinMode(Motor, OUTPUT);
  delay(2000); //Zwei Sekunden Vorlaufzeit bis zur Messung (der Sensor ist etwas träge)
}



void loop() {

  Luftfeuchtigkeit = dht.readHumidity();
  Temperatur = dht.readTemperature();
  Licht = analogRead(pResistor);

  Zeit = millis();
  vergangeZeit = Zeit + 5000;

  Daten();
  debug();
  Ventilator();
  Flasche();
}






void Ventilator() {
  if (Temperatur > 26) {
    digitalWrite(Motor, HIGH);
  } else {
    digitalWrite(Motor, LOW);
  }
}

void Flasche() {
  if (Licht > 150) {
    digitalWrite(Lampe, LOW);
  } else {
    digitalWrite(Lampe, HIGH);
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

void Daten() {

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):

  lcd.setCursor(0, 0);
  lcd.print("Luftfeu: ");
  lcd.print(Luftfeuchtigkeit);
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.setCursor(9, 1);
  lcd.print(Temperatur);

  delay(5000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Licht: ");
  lcd.print(Licht);

  delay(5000);
  lcd.clear();


}
