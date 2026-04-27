#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Moteur.h"

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
#define JOY_X A0
#define JOY_Y A1

#define BTN_URG 2

#define LED_ACTIF 10
#define LED_URG 9

#define ENA 5
#define IN1 44
#define IN2 45

// Moteur
Moteur moteur(ENA, IN1, IN2);

// Machine à états du système
enum EtatSysteme {
  NORMAL,
  URGENCE
};

EtatSysteme etatSys = NORMAL;

// Joystick
int xVal = 512;
int yVal = 512;

const int neutreMin = 450;
const int neutreMax = 550;

int vitesse = 0;

// Bouton urgence
int lastRead = HIGH;
int btnState = HIGH;
unsigned long tDeb = 0;
const unsigned long dDeb = 50;

// LCD 
unsigned long tLcd = 0;
const unsigned long dLcd = 2000;
int page = 0;

// Serial
unsigned long tSerie = 0;
const unsigned long dSerie = 500;

void setup() {
  Serial.begin(9600);

  pinMode(BTN_URG, INPUT_PULLUP);
  pinMode(LED_ACTIF, OUTPUT);
  pinMode(LED_URG, OUTPUT);

  moteur.begin();

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Labo 05");
  lcd.setCursor(0, 1);
  lcd.print("Convoyeur");
}

void loop() {
  unsigned long now = millis();

  lireBouton(now);

  switch (etatSys) {
    case NORMAL:
      lireJoystick();
      gererConvoyeur();
      break;

    case URGENCE:
      moteur.urgence();
      break;
  }

  gererLeds();
  afficherLcd(now);
  afficherSerie(now);
}

void lireJoystick() {
  xVal = analogRead(JOY_X);
  yVal = analogRead(JOY_Y);
}

void lireBouton(unsigned long now) {
  int lecture = digitalRead(BTN_URG);

  if (lecture != lastRead) {
    tDeb = now;
  }

  if (now - tDeb > dDeb) {
    if (lecture != btnState) {
      btnState = lecture;

      if (btnState == LOW) {
        if (etatSys == NORMAL) {
          etatSys = URGENCE;
        } else {
          etatSys = NORMAL;
          moteur.arreter();
          lcd.clear();
        }
      }
    }
  }

  lastRead = lecture;
}

void gererConvoyeur() {
  // Gauche = ralentir
  if (xVal < neutreMin) {
    vitesse -= 3;
  }

  // Droite = accelerer
  if (xVal > neutreMax) {
    vitesse += 3;
  }

  vitesse = constrain(vitesse, 0, 255);
  moteur.setVitesse(vitesse);

  // Haut = avancer
  if (yVal > neutreMax && vitesse > 0) {
    moteur.avancer();
  }
  // Bas = reculer
  else if (yVal < neutreMin && vitesse > 0) {
    moteur.reculer();
  }
  // Joystick au centre = arrêt
  else {
    moteur.arreter();
  }
}

void gererLeds() {
  if (etatSys == URGENCE) {
    digitalWrite(LED_ACTIF, LOW);
    digitalWrite(LED_URG, HIGH);
  } else {
    digitalWrite(LED_ACTIF, moteur.estActif() ? HIGH : LOW);
    digitalWrite(LED_URG, LOW);
  }
}

void afficherLcd(unsigned long now) {
  if (etatSys == URGENCE) {
    lcd.setCursor(0, 0);
    lcd.print("    URGENCE     ");
    lcd.setCursor(0, 1);
    lcd.print("Convoyeur STOP  ");
    return;
  }

  if (now - tLcd >= dLcd) {
    tLcd = now;
    page++;

    if (page > 2) {
      page = 0;
    }

    lcd.clear();
  }

  if (page == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Conv actif:");
    lcd.setCursor(0, 1);
    lcd.print(moteur.estActif() ? "OUI             " : "NON             ");
  }

  else if (page == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Sens convoyeur:");
    lcd.setCursor(0, 1);
    lcd.print(moteur.getSens());
    lcd.print("            ");
  }

  else if (page == 2) {
    lcd.setCursor(0, 0);
    lcd.print("Vitesse:");
    lcd.setCursor(0, 1);
    lcd.print(moteur.getVitesse());
    lcd.print(" /255          ");
  }
}

void afficherSerie(unsigned long now) {
  if (now - tSerie >= dSerie) {
    tSerie = now;

    Serial.print("X=");
    Serial.print(xVal);

    Serial.print(" | Y=");
    Serial.print(yVal);

    Serial.print(" | Vitesse=");
    Serial.print(moteur.getVitesse());

    Serial.print(" | Sens=");
    Serial.print(moteur.getSens());

    Serial.print(" | Actif=");
    Serial.print(moteur.estActif() ? "OUI" : "NON");

    Serial.print(" | Urgence=");
    Serial.println(etatSys == URGENCE ? "OUI" : "NON");
  }
}