/* 
  Programmed by: Antonio Rehwinkel

  Funktionsweise neben den funktionen erklärt. volle Arbeitsweise steht bei der funktion gedruckt()
  bei schreiben neuer funktionen einfügen in chooseProg und text verändern in write_Prog
  bei schreiben darauf achten zeitabhängige sachen per millis zu machen. Kein delay benutzen / nur im notfall delay benutzen
  millis timer googlen!
  
  Used:
       Arduino Uno
       DFPlayer Mini
       Seeed Studio Grove – LCD RGB Backlight
       Transistor for the Lamp
       Joystick
       DHT11
       Photoresitor

  Wiring:
      Joystick=X-Axis:A0
               Y-Axis:A1
               Switch:3
       DHT11=8
       LCD=7
       photoresistor=7
       Lamp/Transistor=13
       DFPlayer Mini=RX:12
                     TX:11

  Sense:
      Playfull assitant, telling light/temp. Will sing to you (with seasons to choose from) and may distract you from work! Keeping you succesfully but distrected ^-^

  Problem:
    wackelkontakt --> bei rütteln geht MP3-Modul aus und nächster song spielt
      
*/
/-----------------------------------
/*
 * Temperatur sensor samt werte speicher
 */
#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float Temperatur;
//-------------------------
/*
 * Variablen für den MP3-Player.
 * benötigt für zeitliche einordnugn aller lieder + pause/stop oder den zufallsmodus
 * zufallsmodus hat sicherung um zwei mal die selbe Zahl nacheinander zu vermeiden
 */
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
SoftwareSerial mySoftwareSerial(11, 12); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
/*
 * Zufallsmodus
 */
int sec = 50;
int MusikRand = 0;
int MusikRandAlt = 0;
long RandZeit = 0;
long twomin = 0; //120000
long randTimer = 0;
/*
 * MusikOrdner
 */
int xMasOrd = 0;
int halloweenOrd = 0;
int voiceOrd = 0;
int currentFile = 0;
/*
 * 
 */
int halloween = 1;
int xMas = 2;
int voice = 3;

int schleifenState = 0;

int maxWarten = 2000;     //300000, 600000
int minWarten = 1000;

int ifCaseRun = true;
int indexGL;
//-------------------------
/*
 * Licht resister samt werte speicher
 */
#define pResistor A3
int Licht;
//-------------------------
/*
 * LCD-Display mit farbsteuerung
 */
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

int colorR = 0;
int colorG = 0;
int colorB = 0;
//-------------------------
/*
 * PWM-Lampe 
 */
#define Lampe 10
boolean debugLampe = false;
int Leuchtkraft = 0;
//-------------------------
/*
 * Timer
 */
long Zeit;
long vergangeneZeit = 0 ;
//long vergangeneZeitMusik = 0;
//-------------------------
/*
 * Joystick
 */
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
/*
 * Menu
 */
boolean FlascheOnOff = false;
int Programm = 0;
int chosenProg = 9;
/*volatile*/ boolean runonce = true;
boolean chooseProgRunOnce = false;
String Programms[] = { "FlascheDIM", "FlascheONOFF", "TempAndHum", "Lichtwert", "Musik", "Spiel"};

//-------------------------------------------------------------------------------------setup
void setup() {
  //init LCD
 lcd.begin(16, 2);   
  // lcd.setRGB(colorR, colorG, colorB);
  lcd.setCursor(0, 0);
  lcd.print("Loading...");
  
  randomSeed(A4);   //set Seed for random (true random möglich)

  /*
   * DFPlayerMini code 
   * wenn nicht vorhanden dann stürzt hier das Programm ab
   */
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    
    lcd.setCursor(0, 0);
    lcd.print("No SD Card");
    
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
  Serial.println(voiceOrd);

  delay(sec);
  //delay(6000);

  //Serial.begin(9600);

  dht.begin(); //temp sensor init

  pinMode(interruptPin, INPUT_PULLUP);  //interrupt für Joystick btn || interupt ruft die angegebene Funktion auf, wenn wert erreicht (für mich wenn btn gedrückt)
  attachInterrupt(digitalPinToInterrupt(interruptPin), gedruckt, CHANGE);

  pinMode(pResistor, INPUT);  // init licht resistor
  delay(2000); //Zwei Sekunden Vorlaufzeit bis zur Messung (der Sensor ist etwas träge)
}
//-------------------------------------------------------------------------------------loop
void loop() {
  while (gedrucktbool) { //wenn knopf des joysticks gedrückt

    //myDFPlayer.pause();
    Menu();             //öffne das menü
    // debug();
    chooseProgRunOnce = false; //setze falsch das ein programm ausgewählt wurde
  }

  if (!chooseProgRunOnce) { //wenn programm noch nicht gelaufen dann clear zuerst das LCD || alter text verschwindet (hatte probleme mit überlagerungen)
    lcd.clear();
  }

  if (!gedrucktbool) {  //wenn joystick gedrückt dann öffne das programm um ein Programm zu starten (xD)

    choose_Prog();
  }
  runonce = true; //das prohjekt ist einmal durchgelaufen (Wichtig für menu() )
  //debug();
}
//-------------------------------------------------------------------------------------Menu
/*
 * der joystick wurde gedrückt, 
 */
void gedruckt() { 
  i = 0;
  buttonstate =  digitalRead(interruptPin); 

/*
 * case switch für runonce + abdeckung verschiedener programm fälle die vorher gelaufen sein könnten
 * durch runnonce wird sichergestellt das das menu beim starten offen ist
 * 
 * knopf wird das erste mal gedrückt --> erste if verknüpfung geht auf (noch nicht gelaufen und btn losgelassen)
 * setze gedrucktbool war, dadurch wird das menu geöffnet. Hier kann der Spieler seine Programme auswählen und laufen lassen
 * 
 * wird der knopf ein weiteres mal gedrückt, so setze gerucktbool auf false
 * das menu schliest sich über dem letzen ausgewähltem programm (siehe write_prog() / choose_Prog() oder Menu()
 * 
 * da gedrucktbool falsch ist starte das ausgewählte programm!
 */
  if (runnonce == 0 && buttonstate == LOW) {  //wenn das programm noch nicht lief(zB beim start des arduinos) und der btn losgelassen(debounce) wurde dann 
    gedrucktbool = true;  // knopf gedrückt war
    myDFPlayer.pause();   //pausiere den MP3Player (im menu nur nervig)
    runnonce = 1;         //das programm lief (siehe menu())
    twomin = 0;           //?
  }
  else if (runnonce == 1 && buttonstate == LOW) { //das programm ist schon einmal gelaufen und der btn losgelassen(debounce) dann
    gedrucktbool = false; // es wurde noch nicht gedrückt == enter choose_prog (siehe loop)

    runnonce = 0; //das programm lief noch nicht == enter menu
  }

  while (buttonstate == LOW) { // weiterer debounce
    i++;
    delay(10);
    buttonstate =  digitalRead(interruptPin);
    // debug();
  }
  // debug();

}
//----------------------------------------
/*
 * Menu um programm auszuwählen. write_Prog um zu scrollen
 */
void Menu() {

  if (runonce) { // programm ist noch nicht gelaufen 
    lcd.clear();    
    lcd.setCursor(0, 0);
    lcd.print("Menu"); //zeige an das menu geöffnet
    runonce = false;
  }

  VrYVar = analogRead(VrY); //lese axen des joysticks

  if (VrYVar > 900) { //joystick nach unten
    Write_Prog(); //scroll nach unten mit anzeige der auszuwählenden programme auf dem bildschirm
    // debug();
    while (VrYVar > 900) {  //während jyostick noch unten warte (debounce)
      VrYVar = analogRead(VrY);
      delay(10);
      //debug();
    }
  }
}
//----------------------------------------
/*
 * das im Menu/write_prog gewählte Programm wird hier ausgewählt und ausgeführt
 */
void choose_Prog() { 

  /*
     lcd.setCursor(0,0);
     lcd.print("Play Programm: ");
     lcd.setCursor(0,1);
     lcd.print(Programms[chosenProg]);
  */

  //VrXVar = analogRead(VrX);
  //debug();

  // if (VrXVar > 900) {
  //lcd.clear();

  switch (chosenProg) { // von write prog gesetzt wenn der interrupt skipt, so wird ausgweählt und hier ausgeführt

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
      Musik(1);
      break;

    case 5:
      Musik(3);
      break;

    case 6:
      Musik(2);
      break;

    case 7:
      Spiel();
      break;

    case 8:
      Musik(4);
      break;

    case 9:
      no_Prog();
      break;

  }
  chooseProgRunOnce = true;
}
//----------------------------------------
/*
 * Programm stellt die Position in der Liste dar.
 * je nach dem wie weit gescrollt wurde, zeigt Write_Prog das entsprechende Programm zur auswahl an.
 * Wird mit dem Joystick btn bestätigt skippt der Interrupt den while loop des menu. Somit gilt die Zahl des letzen heir geöffnetem Programm als gewähltem programm
 */
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
/*
 * falls kein programm gewählt (nach boot zB)
 */
void no_Prog() {
  digitalWrite(Lampe, LOW);
  lcd.setCursor(0, 0);
  lcd.print("No Prog chosen");
  lcd.setCursor(0, 1);
  lcd.print("Choose Programm");
  //  Serial.println("Programm");
}

//-------------------------------------------------------------------------------------Programme
/*
 * Hier stehen die Programme die ausgeführt werden können. Diese stehen in WriteProg zum scrollen und in choose_prog zum ausführen dabei
 */
//-------------------------------------------------------------------------------------Sensorik
void  Flasche_DIM() {
  Licht = analogRead(pResistor);

  Leuchtkraft = map(Licht, 1, 762, 255, 1);
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
void Musik(int indexPR) {
   
  indexGL = indexPR; 
  int runningMusik;
  int intrunningMusik2;
  ifCaseRun = true;

  Zeit = millis();
 digitalWrite(Lampe, LOW);
  //---------------------------------------------------------------time
    if (twomin < Zeit) {
      runningMusik = myDFPlayer.readState();
      delay(500);
      intrunningMusik2 = myDFPlayer.readState();
      twomin = Zeit + 2000;
      Serial.println("Time set");
      delay(500);
      debug();
    }
  //---------------------------------------------------------------time
  //---------------------------------------------------------------still playing
    if (runningMusik == 512 && ifCaseRun) {
      // RandZeit = 2;
 
    SWITCHCASE();
    
    }
  //---------------------------------------------------------------still playing
  //---------------------------------------------------------------paused
    if (intrunningMusik2 == 514 && ifCaseRun) {
      // RandZeit = 2;
   
      SWITCHCASE();
    }
  //---------------------------------------------------------------paused
}
//----------------------------------------
void SWITCHCASE (){

    Serial.println("Musik running");

    switch ( indexGL) {
      case 1:                                 //Musik
        do {
          MusikRand = random(1, 25);
          Serial.println("FUUUUUCK3");
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

      
        break;

      case 2:                                 //Halloween
        do {
          MusikRand = random(1, halloweenOrd);
          Serial.println("FUUUUUCK2");
        } while (MusikRand == MusikRandAlt);

        MusikRandAlt = MusikRand;

        myDFPlayer.playFolder(/*halloween*/ 1, MusikRand);
        delay(sec);
        currentFile = myDFPlayer.readCurrentFileNumber();

        lcd.setCursor(0, 0);
        lcd.print("Halloween:  ");
        lcd.setCursor(0, 1);
        lcd.print(currentFile);
        delay(sec);

    
        break;

      case 3:                                 //Christmas
        do {
          MusikRand = random(1, xMasOrd);
          Serial.println("FUCK");

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

  
        break;

      case 4:                           //turret voice || add the random timing
          do {
            MusikRand = random(1, voiceOrd);
            Serial.println(voiceOrd);
          } while (MusikRand == MusikRandAlt);
          MusikRandAlt = MusikRand;

          myDFPlayer.playFolder(/*turret*/  3, MusikRand);
          delay(sec);

          currentFile = myDFPlayer.readCurrentFileNumber();

          lcd.setCursor(0, 0);
          lcd.print("Turret:  ");
          lcd.setCursor(0, 1);
          lcd.print( currentFile);

         
        break;
    }
    ifCaseRun = false;
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
  /*
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

    } else {
    delay(2000);
    }
    }

    /*
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

 // Serial.print("MP3State: ");
 // Serial.println(myDFPlayer.readState());
  /*
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
  //Serial.println(twomin - Zeit);

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
