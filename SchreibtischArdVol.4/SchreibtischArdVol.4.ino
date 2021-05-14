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
#define VrX  A0
#define VrY  A1
const byte interruptPin = 2;
int VrXVar;
int VrYVar;
int SWVar = 0;
volatile boolean gedrucktbool = false;
int runnonce = 0;
int buttonstate = 0;
int i  = 0;
//-----------------------
boolean FlascheOnOff = false;
int Programm = 0;
int chosenProg = 9;
/*volatile*/ boolean runonce = true;
boolean chooseProgRunOnce = false;
String Programms[] = { "FlascheDIM", "FlascheONOFF", "TempAndHum", "Lichtwert", "Musik", "Spiel"};



void setup() {

 Serial.begin(9600);

  dht.begin();

  lcd.begin(16, 2);
  // lcd.setRGB(colorR, colorG, colorB);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), gedruckt, CHANGE);

  pinMode(pResistor, INPUT);
  delay(2000); //Zwei Sekunden Vorlaufzeit bis zur Messung (der Sensor ist etwas träge)
}



void loop() {


  while (gedrucktbool) {
    Menu();
    debug();
    chooseProgRunOnce = false;
  }

  if (!chooseProgRunOnce) {
    lcd.clear();
  }

  if (!gedrucktbool) {
    chooseProg();
  }
  runonce = true;
  debug();
}

//-------------------------------------------------------------------------------------
void gedruckt() {
  i = 0;
  buttonstate =  digitalRead(interruptPin);
  if (runnonce == 0 && buttonstate == LOW) {
    gedrucktbool = true;
    runnonce = 1;
  }
  else if (runnonce == 1 && buttonstate == LOW) {
    gedrucktbool = false;

    runnonce = 0;
  }

  while (buttonstate == LOW) {
    i++;
    delay(10);
    buttonstate =  digitalRead(interruptPin);
    // debug();
  }
  // debug();
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

  if (VrYVar > 900) {
    WriteProg();
    debug();
    while (VrYVar > 900) {
      VrYVar = analogRead(VrY);
      delay(10);
      debug();
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

  //VrXVar = analogRead(VrX);
  debug();

  // if (VrXVar > 900) {
  //lcd.clear();

  switch (chosenProg) {

    case 0:
      FlascheDIM();
      break;

    case 1:
      FlascheONOFF();
      break;

    case 2:
      TempAndHum();
      break;

    case 3:
      Lichtwert();
      break;

    case 4:
      Musik();
      break;

    case 5:
      Spiel();
      break;

    case 9:
      noProg();
      break;

  }
  chooseProgRunOnce = true;
}
//}
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
      lcd.print("Temp");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("Temp");
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
      Programm = 0;
      chosenProg = 5;
      break;
  }
}
//--------------------------------------------
void noProg() {
  digitalWrite(Lampe, LOW);
  lcd.setCursor(0, 0);
  lcd.print("No Prog chosen");
  lcd.setCursor(0, 1);
  lcd.print("Choose Programm");
  Serial.println("Programm");
}
//-------------------------------------------
void  FlascheDIM() {
  Licht = analogRead(pResistor);

  Leuchtkraft = map(Licht, 0, 762, 255, 0);
  analogWrite(Lampe, Leuchtkraft);

  Zeit = millis();

  if (vergangeneZeit < Zeit) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dim-Wert:");
    lcd.setCursor(0, 1);
    lcd.print(Leuchtkraft);

    vergangeneZeit = Zeit + 250;
  }

}
//--------------------------------------------
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
  digitalWrite(Lampe, LOW);
  Temperatur = dht.readTemperature();

  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.setCursor(0, 1);
  lcd.print(Temperatur);
}
//-------------------------------------------
void Lichtwert() {
  digitalWrite(Lampe, LOW);
  Licht = analogRead(pResistor);
  lcd.setCursor(0, 0);
  lcd.print("Lichtwert: ");
  lcd.setCursor(0, 1);
  lcd.print(Licht);
}
//------------------------------------------
void Musik() {
  digitalWrite(Lampe, LOW);
  /*
     Work in Progress
  */
}
//------------------------------------------
void Spiel() {
  digitalWrite(Lampe, LOW);
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
  */
  Serial.print("Licht: ");
  Serial.println(Licht);
  Serial.print("Leuchtstärke: ");
  Serial.println(Leuchtkraft);
  /*
        if(debugLampe == false){
          Serial.println("Lampe aus");
        }else{
          Serial.println("Lampe an");
        }
  */
  // Serial.println(VrXVar);
  //Serial.println(VrYVar);
  //Serial.println(gedrucktbool);
  //Serial.println(i);

  //Serial.println(Programm);
  //Serial.println(Programms[chosenProg]);

  //Serial.println(Zeit);

}


//-----------------------------------------------------------------


/*

  void setup() {
  pinMode( ledPin, OUTPUT);
  pinMode(knopf, INPUT_PULLUP);

  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(knopf), blink, CHANGE);

  }

  void loop() {
  if (Lampe) {
    Serial.println("AN");
    digitalWrite(ledPin, HIGH);

  } else {
    Serial.println("AUS");
    digitalWrite(ledPin, LOW);
  }
  // Serial.println(digitalRead(kmopf));
  }

  void blink() {

  }
*/
