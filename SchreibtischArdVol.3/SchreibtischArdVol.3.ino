#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float Luftfeuchtigkeit;
float Temperatur;
//-------------------------
#define pResistor A3
int Licht;
//-------------------------
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

int colorR = 0;
int colorG = 0;
int colorB = 0;
//-------------------------
#define Lampe 10
boolean debugLampe = false;
int Leuchtkraft = 0;
//-------------------------
long Zeit;
long vergangeneZeit = 0 ;
//-------------------------
#define VrX  0
#define VrY  1
#define SW  7
int VrXVar;
int VrYVar;
int SWVar = 0;
boolean wurdegedruckt = false;
boolean gedrucktbool = false;
//-----------------------
boolean FlascheOnOff = false;
int Programm = 0;
int chosenProg = 0;
boolean runonce;
String Programms[] = { "FlascheDIM", "FlascheONOFF", "TempAndHum", "Lichtwert", "Musik", "Spiel"};

void setup() {

  Serial.begin(9600);

  dht.begin();

  lcd.begin(16, 2);
  // lcd.setRGB(colorR, colorG, colorB);

  pinMode(SW, INPUT);

  pinMode(pResistor, INPUT);
  delay(2000); //Zwei Sekunden Vorlaufzeit bis zur Messung (der Sensor ist etwas träge)
}



void loop() {

  gedruckt();

  while (gedrucktbool) {
    Menu();
    gedruckt();

    debug();
  }

  chooseProg();

  debug();
}

//-------------------------------------------------------------------------------------
void gedruckt() {

  SWVar = digitalRead(SW);

  if (SWVar == LOW) {

    while (SWVar == LOW) {
      delay(10);
      SWVar = digitalRead(SW);
    }

    if (wurdegedruckt) {
      wurdegedruckt = false;
     //
     lcd.clear();
    } else {
      wurdegedruckt = true;
    }
  }

  switch (wurdegedruckt) {
    case true:
      gedrucktbool = true;
      break;
    case false:
      gedrucktbool = false;
      runonce = true;
      break;
  }
}
//----------------------------------------
void Menu() {

  if (runonce) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Menu");
    runonce = false;
  }

  VrYVar = analogRead(VrY);

  if (VrYVar > 1000) {
    WriteProg();

    while (VrYVar > 1000) {
      VrYVar = analogRead(VrY);
      delay(10);
    }
  }
}
//--------------------------------------------
void chooseProg() {

  /*
     lcd.setCursor(0,0);
     lcd.print("Play Programm: ");
     lcd.setCursor(0,1);
     lcd.print(Programms[chosenProg]);
*/

  VrXVar = analogRead(VrX);
 debug();

  if (VrXVar > 1000) {
//lcd.clear();
    switch (chosenProg) {

      case 0:
        while (!gedrucktbool) {
          FlascheDIM();
          gedruckt();
        }

        break;

      case 1:
        while (!gedrucktbool) {
          FlascheONOFF();
          gedruckt();
        }
        break;

      case 2:
        while (!gedrucktbool) {
          TempAndHum();
          gedruckt();
        }
        break;

      case 3:
        while (!gedrucktbool) {
          Lichtwert();
          gedruckt();
        }
        break;

      case 4:
        while (!gedrucktbool) {
          Musik();
          gedruckt();
        }
        break;

      case 5:
        while (!gedrucktbool) {
          Spiel();
          gedruckt();
        }
        break;

    }
  }
}
//--------------------------------------------
void WriteProg() {
  switch (Programm) {
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("FlaschenDIM");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("FlaschenDIM");
      Programm++;
      chosenProg = 0;
      break;

    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Flasche ON/OFF");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("FlaschenONOFF");
      Programm++;
      chosenProg = 1;
      break;

    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp and Hum");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("TempAndHum");
      Programm++;
      chosenProg = 2 ;
      break;

    case 3:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Lichtwert");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("Lichtwert");
      Programm++;
      chosenProg = 3;
      break;

    case 4:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Musik");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("Musik");
      Programm++;
      chosenProg = 4;
      break;

    case 5:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Spiel");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("Spiel");
      Programm = 1;
      chosenProg = 5;
      break;
  }
}
//--------------------------------------------
void  FlascheDIM() {
  Licht = analogRead(pResistor);
  Zeit = millis();

  if (vergangeneZeit < Zeit) {
    Leuchtkraft = map(Licht, 1, 761, 255, 1);
    analogWrite(Lampe, Leuchtkraft);
    vergangeneZeit = vergangeneZeit + 250;
  }
}
//--------------------------------------------                          // in Progress
void FlascheONOFF() {
  if (!FlascheOnOff) {
    digitalWrite(Lampe, HIGH);
    FlascheOnOff = true;
  } else {
    digitalWrite(Lampe, LOW);
    FlascheOnOff = false;
  }
}
//-------------------------------------------
void TempAndHum() {
  Luftfeuchtigkeit = dht.readHumidity();
  Temperatur = dht.readTemperature();

  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Luftfeu: ");
  lcd.print(Luftfeuchtigkeit);
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.setCursor(9, 1);
  lcd.print(Temperatur);
}
//-------------------------------------------
void Lichtwert() {
  Licht = analogRead(pResistor);
  Leuchtkraft = map(Licht, 1, 761, 255, 1);
  lcd.setCursor(0, 0);
  lcd.print("Lichtwert: ");
  lcd.setCursor(0, 1);
  lcd.print(Leuchtkraft);
}
//------------------------------------------
void Musik() {
  /*
     Work in Progress
  */
}
//------------------------------------------
void Spiel() {
  /*
     Work in progress
  */
}
//------------------------------------------------------------------------------------




void debug() {
  /*
    Serial.print("Luftfeuchtigkeit: ");
    Serial.println(Luftfeuchtigkeit);
    Serial.print("Temperatur: ");
    Serial.println(Temperatur);

    Serial.print("Licht: ");
      Serial.println(Licht);
      Serial.println("Leuchtstärke: ");

    Serial.println(Leuchtkraft);

      if(debugLampe == false){
        Serial.println("Lampe aus");
      }else{
        Serial.println("Lampe an");
      }
  */
  // Serial.println(VrXVar);
  //Serial.println(VrYVar);
  //Serial.println(SWVar);

  //Serial.println(Programm);
  Serial.println(Programms[chosenProg]);

  //Serial.println(wurdegedruckt);

}
