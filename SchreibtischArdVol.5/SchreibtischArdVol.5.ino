#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float Temperatur;
//-------------------------
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
SoftwareSerial mySoftwareSerial(11, 12); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

int sec = 20;
int MusikRand = 0;
int MusikRandAlt = 0;
long RandZeit = 0;
long twomin = 0; //120000

int xMasOrd = 0;
int halloweenOrd = 0;
int voiceOrd = 0;
int currentFile = 0;

int halloween = 1;
int xMas = 2;
int voice = 3;

int schleifenState = 0;

int maxWarten = 2000;     //300000, 600000
int minWarten = 1000;
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
//long vergangeneZeitMusik = 0;
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

//-------------------------------------------------------------------------------------setup
void setup() {

  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.setTimeOut(100); //Set serial communictaion time out 500ms
  delay(sec);
  myDFPlayer.volume(25);  //Set volume value (0~30).
  delay(sec);
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  delay(sec);
  myDFPlayer.disableLoopAll();
  delay(sec);
  xMasOrd = myDFPlayer.readFileCountsInFolder(xMas);
  delay(sec);
  halloweenOrd = myDFPlayer.readFileCountsInFolder(halloween);
  delay(sec);
  voiceOrd = myDFPlayer.readFileCountsInFolder(voice);
  delay(sec);

  //Serial.begin(9600);

  dht.begin();

  lcd.begin(16, 2);
  // lcd.setRGB(colorR, colorG, colorB);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), gedruckt, CHANGE);

  pinMode(pResistor, INPUT);
  delay(2000); //Zwei Sekunden Vorlaufzeit bis zur Messung (der Sensor ist etwas träge)
}
//-------------------------------------------------------------------------------------loop
void loop() {


  while (gedrucktbool) {
    myDFPlayer.pause();
    Menu();
    debug();
    chooseProgRunOnce = false;
  }

  if (!chooseProgRunOnce) {
    lcd.clear();
  }

  if (!gedrucktbool) {
    choose_Prog();
  }
  runonce = true;
  debug();
}
//-------------------------------------------------------------------------------------Software für Bild
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
    Write_Prog();
    debug();
    while (VrYVar > 900) {
      VrYVar = analogRead(VrY);
      delay(10);
      debug();
    }
  }
}
//----------------------------------------
void choose_Prog() {

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
      Flasche_DIM();
      break;

    case 1:
      Flasche_ON_OFF();
      break;

    case 2:
      Temp_and_Hum();
      break;

    case 3:
      Lichtwert();
      break;

    case 4:
      Musik();
      break;

    case 5:
      Christmas();
      break;

    case 6:
      Halloween();
      break;

    case 7:
      Spiel();
      break;

    case 8:
      Random_Reaction();
      break;

    case 9:
      no_Prog();
      break;

  }
  chooseProgRunOnce = true;
}
//----------------------------------------
void Write_Prog() {
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
      lcd.print("Christmas");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("Christmas");
      Programm++;
      chosenProg = 5;
      break;


    case 6:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Halloween");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("Halloween");
      Programm++;
      chosenProg = 6;
      break;

    case 7:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Spiel");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("Spiel");
      Programm++;
      chosenProg = 7;
      break;

    case 8:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Turret");
      lcd.setCursor(0, 1);
      lcd.print("Choose -->");
      Serial.println("Turret");
      Programm = 0;
      chosenProg = 8;
      break;

  }
}
//----------------------------------------
void no_Prog() {
  digitalWrite(Lampe, LOW);
  lcd.setCursor(0, 0);
  lcd.print("No Prog chosen");
  lcd.setCursor(0, 1);
  lcd.print("Choose Programm");
  //  Serial.println("Programm");
}
//-------------------------------------------------------------------------------------Sensorik
void  Flasche_DIM() {
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
//----------------------------------------
void Flasche_ON_OFF() {
  if (!FlascheOnOff) {
    digitalWrite(Lampe, HIGH);
    FlascheOnOff = true;
  } else {
    digitalWrite(Lampe, LOW);
    FlascheOnOff = false;
  }
}
//----------------------------------------
void Temp_and_Hum() {
  digitalWrite(Lampe, LOW);
  Temperatur = dht.readTemperature();

  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.setCursor(0, 1);
  lcd.print(Temperatur);
}
//----------------------------------------
void Lichtwert() {
  digitalWrite(Lampe, LOW);
  Licht = analogRead(pResistor);
  lcd.setCursor(0, 0);
  lcd.print("Lichtwert: ");
  lcd.setCursor(0, 1);
  lcd.print(Licht);
}
//-------------------------------------------------------------------------------------MP3
void Musik() {
  digitalWrite(Lampe, LOW);
  Zeit = millis();

  //RandZeit = 1;
  debug();

  if (twomin < Zeit) {

    // RandZeit = 2;
    debug();

    if (myDFPlayer.readState() == 512 || myDFPlayer.readState() == 514) {
      RandZeit = random(minWarten, maxWarten);
      debug();
      delay(RandZeit);

      do {
        MusikRand = random(1, 25);
      } while (MusikRand == MusikRandAlt);
      MusikRandAlt = MusikRand;

      myDFPlayer.play(MusikRand);
      delay(sec);
      currentFile =  myDFPlayer.readCurrentFileNumber();

      lcd.setCursor(0, 0);
      lcd.print("MP3:  ");
      lcd.setCursor(0, 1);
      lcd.print( currentFile);
      delay(sec);

      twomin = Zeit + 120000;
    } else {
      delay(2000);
    }
  }
}
//----------------------------------------
void Halloween() {

  digitalWrite(Lampe, LOW);
  Zeit = millis();

  //RandZeit = 1;
  debug();

  if (twomin < Zeit) {

    // RandZeit = 2;
    debug();

    if (myDFPlayer.readState() == 512 || myDFPlayer.readState() == 514) {
      RandZeit = random(minWarten, maxWarten);
      debug();
      delay(RandZeit);

      do {
        MusikRand = random(1, halloweenOrd);
      } while (MusikRand == MusikRandAlt);
      MusikRandAlt = MusikRand;
      myDFPlayer.playFolder(/*halloween*/ 1, MusikRand);
      delay(sec);
      currentFile = myDFPlayer.readCurrentFileNumber();

      lcd.setCursor(0, 0);
      lcd.print("Halloween:  ");
      lcd.setCursor(0, 1);
      lcd.print( currentFile);
      delay(sec);

      twomin = Zeit + 120000;
    } else {
      delay(2000);
    }
  }
}
//----------------------------------------
void Christmas() {

  digitalWrite(Lampe, LOW);
  Zeit = millis();

  //RandZeit = 1;
  debug();

  if (twomin < Zeit) {

    // RandZeit = 2;
    debug();

    if (myDFPlayer.readState() == 512 || myDFPlayer.readState() == 514) {
      RandZeit = random(minWarten, maxWarten); //300000, 600000
      debug();
      delay(RandZeit);

      do {
        MusikRand = random(1, xMasOrd);
      } while (MusikRand == MusikRandAlt);
      MusikRandAlt = MusikRand;

      myDFPlayer.playFolder(/*xMas*/ 2, MusikRand);
      delay(sec);
      currentFile = myDFPlayer.readCurrentFileNumber();

      lcd.setCursor(0, 0);
      lcd.print("xMas:  ");
      lcd.setCursor(0, 1);
      lcd.print( currentFile);
      delay(sec);

      twomin = Zeit + 120000;
    } else {
      delay(2000);
    }
  }





}
//-------------------------------------------------------------------------------------Spiel
void Spiel() {
  digitalWrite(Lampe, LOW);
  /*
     Work in progress
  */
}
//-------------------------------------------------------------------------------------RandomReaction
void Random_Reaction() {
  digitalWrite(Lampe, LOW);
  Zeit = millis();
schleifenState = 1;
  //RandZeit = 1;
  debug();

  if (twomin < Zeit) {
schleifenState = 200;
    // RandZeit = 2;
    debug();

    if (myDFPlayer.readState() == 512 || myDFPlayer.readState() == 514) {
      Zeit = millis();
      twomin = Zeit + 120000;
      schleifenState = 30000;
      RandZeit = random(minWarten, maxWarten);
debug();
      delay(RandZeit);
debug();                    //hier stoppt das programm nach dem 2ten Durchlauf || Der RAM ist frei. Millis/twomin zählen nicht weiter
      do {
        MusikRand = random(1, voiceOrd);
debug();
      } while (MusikRand == MusikRandAlt);
      MusikRandAlt = MusikRand;
debug();
      myDFPlayer.playFolder(/*turret*/ 3, MusikRand);
      delay(sec);
      currentFile = myDFPlayer.readCurrentFileNumber();
debug();
      lcd.setCursor(0, 0);
      lcd.print("Turret:  ");
      lcd.setCursor(0, 1);
      lcd.print( currentFile);
      delay(sec);
debug();
      
    } else {
      delay(2000);
    }
  }

  /**
     Bildschirm
  */



}
//-------------------------------------------------------------------------------------RAM
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
//-------------------------------------------------------------------------------------debug
void debug() {
 /*
    Serial.print("MP3State: ");
    Serial.println(myDFPlayer.readState());

    Serial.print("WarteZeit: ");
     Serial.println(RandZeit); 
    Serial.print("ZWEIMIN: ");
     Serial.println(twomin);
 
  Serial.print("DIF: ");
  Serial.println(twomin - Zeit);
  
      Serial.print("Temperatur: ");
      Serial.println(Temperatur);

      Serial.print("Licht: ");
      Serial.println(Licht);
      Serial.print("Leuchtstärke: ");
      Serial.println(Leuchtkraft);

          if(debugLampe == false){
            Serial.println("Lampe aus");
          }else{
            Serial.println("Lampe an");
          }

      Serial.println(VrXVar);
      Serial.println(VrYVar);
      Serial.println(gedrucktbool);
      Serial.println(i);

      Serial.println(Programm);
      Serial.println(Programms[chosenProg]);

    Serial.print("zeit: ");
     Serial.println(Zeit);
  */
  Serial.println(freeRam());
}




//-------------------------------------------------------------------------------------JuNk CoDe
/*
    if(myDFPlayer.readState() == 512 && MusikRunOnce){
    RandZeit = random(300000, 600000);
      vergangeneZeitMusik = Zeit + RandZeit;
      MusikRunOnce = false;
    }

    if (vergangeneZeitMusik < Zeit && !MusikRunOnce) {

      do {
        MusikRand = random(1, 25);
      } while (MusikRand == MusikRandAlt);
      MusikRandAlt = MusikRand;

      myDFPlayer.play(MusikRand); //!Achtung!
      delay(sec);

     MusikRunOnce = true;

      lcd.setCursor(0, 0);
      lcd.print("MP3: ");
      lcd.setCursor(0, 1);
      lcd.print( myDFPlayer.readCurrentFileNumber());
      delay(sec);
    }
*/
/*
  void Halloween() {
  digitalWrite(Lampe, LOW);
  Zeit = millis();

  if(myDFPlayer.readState() == 512 && MusikRunOnce){
  RandZeit = random(300000, 600000);
    vergangeneZeitMusik = Zeit + RandZeit;
    MusikRunOnce = false;
  }

  if (vergangeneZeitMusik < Zeit && !MusikRunOnce) {

    do {
      MusikRand = random(1, myDFPlayer.readFileCountsInFolder(2));
    } while (MusikRand == MusikRandAlt);
    MusikRandAlt = MusikRand;

    myDFPlayer.playFolder(1, MusikRand);
    delay(sec);

   MusikRunOnce = true;

    lcd.setCursor(0, 0);
    lcd.print("Song(Halloween): ");
    lcd.setCursor(0, 1);
    lcd.print( myDFPlayer.readCurrentFileNumber());
    delay(sec);
  }
  }
*/
/*

  if (twomin < Zeit ) {

    if (myDFPlayer.readState() == 512) {

      if (!MusikRunOnce) {
        RandZeit = random(300000, 600000);
        vergangeneZeitMusik = Zeit + RandZeit;
        MusikRunOnce = true;
      }

      if (vergangeneZeitMusik < Zeit) {

        MusikRunOnce = false;
        twomin = Zeit + 120000;

        do {
          MusikRand = random(1, myDFPlayer.readFileCountsInFolder(1));
        } while (MusikRand == MusikRandAlt);
        MusikRandAlt = MusikRand;

        myDFPlayer.playFolder(1, MusikRand);
        delay(sec);

        lcd.setCursor(0, 0);
        lcd.print("Song(Xmas): ");
        lcd.setCursor(0, 1);
        lcd.print( myDFPlayer.readCurrentFileNumber());
        delay(sec);
      }

    } else
    {
      twomin = Zeit + 1000;
    }

  }
  /*
    if (myDFPlayer.readState() == 512 && vergangeneZeitMusik < Zeit) {

      do {
        MusikRand = random(1, myDFPlayer.readFileCountsInFolder(2));
      } while (MusikRand == MusikRandAlt);
      MusikRandAlt = MusikRand;

      myDFPlayer.playFolder(2, MusikRand);
      delay(sec);

      MusikRunOnce = true;

      lcd.setCursor(0, 0);
      lcd.print("Song(Xmas): ");
      lcd.setCursor(0, 1);
      lcd.print( myDFPlayer.readCurrentFileNumber());
      delay(sec);

      RandZeit = random(300000, 600000);
      vergangeneZeitMusik = Zeit + RandZeit;
    }
*/
/*

  if (twomin < Zeit ) {

    if (myDFPlayer.readState() == 512) {

      if (!MusikRunOnce) {
        RandZeit = random(300000, 600000);
        vergangeneZeitMusik = Zeit + RandZeit;
        MusikRunOnce = true;
      }

      if (vergangeneZeitMusik < Zeit) {

        MusikRunOnce = false;
        twomin = Zeit + 120000;

        do {
          MusikRand = random(1, myDFPlayer.readFileCountsInFolder(2));
        } while (MusikRand == MusikRandAlt);
        MusikRandAlt = MusikRand;

        myDFPlayer.playFolder(2, MusikRand);
        delay(sec);

        lcd.setCursor(0, 0);
        lcd.print("Song(Xmas): ");
        lcd.setCursor(0, 1);
        lcd.print( myDFPlayer.readCurrentFileNumber());
        delay(sec);
      }

    } else
    {
      twomin = Zeit + 1000;
    }

  }
  /*
    if (myDFPlayer.readState() == 512 && vergangeneZeitMusik < Zeit) {

      do {
        MusikRand = random(1, myDFPlayer.readFileCountsInFolder(2));
      } while (MusikRand == MusikRandAlt);
      MusikRandAlt = MusikRand;

      myDFPlayer.playFolder(2, MusikRand);
      delay(sec);

      MusikRunOnce = true;

      lcd.setCursor(0, 0);
      lcd.print("Song(Xmas): ");
      lcd.setCursor(0, 1);
      lcd.print( myDFPlayer.readCurrentFileNumber());
      delay(sec);

      RandZeit = random(300000, 600000);
      vergangeneZeitMusik = Zeit + RandZeit;
    }*/
/*      alt code


  if (twomin < Zeit ) {                              //WarteZeit für alle Funktionen geändert. | Kein lcd warteschirm | Soundcheck

    if (myDFPlayer.readState() == 512) {

      if (!MusikRunOnce) {
        RandZeit = random(300000, 600000);
        vergangeneZeitMusik = Zeit + RandZeit;
        MusikRunOnce = true;
      }

      if (vergangeneZeitMusik < Zeit) {

        MusikRunOnce = false;
        twomin = Zeit + 120000;

        do {
          MusikRand = random(1, 25);
        } while (MusikRand == MusikRandAlt);
        MusikRandAlt = MusikRand;

        myDFPlayer.play(MusikRand);
        delay(sec);

        lcd.setCursor(0, 0);
        lcd.print("Song(Xmas): ");
        lcd.setCursor(0, 1);
        lcd.print( myDFPlayer.readCurrentFileNumber());
        delay(sec);
      }

    } else
    {
      twomin = Zeit + 1000;
    }

  }*/
