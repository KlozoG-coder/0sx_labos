#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// -------------------- LCD --------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// -------------------- SERVO --------------------
Servo porte;

// -------------------- PINS --------------------
#define TRIG_PIN 9
#define ECHO_PIN 10
#define SERVO_PIN 7
#define BTN_OPEN 2
#define BTN_STOP 3

// -------------------- ETATS --------------------
enum Etat {
  FERME,
  OUVERTURE,
  OUVERT,
  FERMETURE,
  URGENCE
};

Etat etat = FERME;

// -------------------- VARIABLES --------------------
int dist = 999;
int pos = 0;

int lastOpen = HIGH;
int lastStop = HIGH;
int openState = HIGH;
int stopState = HIGH;

unsigned long tOpen = 0;
unsigned long tMove = 0;
unsigned long tDebOpen = 0;
unsigned long tDebStop = 0;
unsigned long tPrint = 0;

// -------------------- CONSTANTES --------------------
const int seuil = 20;                    // distance d'ouverture en cm
const int posFerme = 0;                  // porte fermée
const int posOuvert = 170;               // porte ouverte
const int pasServo = 2;                  // vitesse d'ouverture / fermeture
const unsigned long delaiMove = 20;      // mouvement servo
const unsigned long delaiOpen = 10000;   // attente porte ouverte : 10 s
const unsigned long dDeb = 50;           // anti-rebond
const unsigned long dPrint = 300;        // debug série

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BTN_OPEN, INPUT_PULLUP);
  pinMode(BTN_STOP, INPUT_PULLUP);

  porte.attach(SERVO_PIN);
  porte.write(posFerme);
  pos = posFerme;

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

// -------------------- LOOP --------------------
void loop() {
  lireDistance();
  gererBtnOpen();
  gererBtnStop();
  gererEtat();
  afficherLCD();
  debugSerie();
}

// -------------------- LECTURE DISTANCE --------------------
void lireDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duree = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duree == 0) {
    dist = 999;
  } else {
    dist = duree * 0.034 / 2;
  }
}

// -------------------- BOUTON OUVERTURE --------------------
void gererBtnOpen() {
  int lecture = digitalRead(BTN_OPEN);

  if (lecture != lastOpen) {
    tDebOpen = millis();
  }

  if (millis() - tDebOpen > dDeb) {
    if (lecture != openState) {
      openState = lecture;

      // INPUT_PULLUP : clic = LOW
      if (openState == LOW) {
        // si fermée ou en fermeture, on force l'ouverture
        if (etat == FERME || etat == FERMETURE) {
          etat = OUVERTURE;
        }
        // si déjà en ouverture, on ne fait rien
      }
    }
  }

  lastOpen = lecture;
}

// -------------------- BOUTON URGENCE --------------------
void gererBtnStop() {
  int lecture = digitalRead(BTN_STOP);

  if (lecture != lastStop) {
    tDebStop = millis();
  }

  if (millis() - tDebStop > dDeb) {
    if (lecture != stopState) {
      stopState = lecture;

      // INPUT_PULLUP : clic = LOW
      if (stopState == LOW) {
        if (etat != URGENCE) {
          // arrêt complet, garde la position actuelle
          etat = URGENCE;
        } else {
          // redémarrage : retour à l'état fermé
          pos = posFerme;
          porte.write(pos);
          etat = FERME;
        }
      }
    }
  }

  lastStop = lecture;
}

// -------------------- MACHINE A ETATS --------------------
void gererEtat() {
  switch (etat) {

    case FERME:
      // ouverture automatique par détection
      if (dist <= seuil) {
        etat = OUVERTURE;
      }
      break;

    case OUVERTURE:
      if (millis() - tMove >= delaiMove) {
        tMove = millis();

        pos += pasServo;
        if (pos > posOuvert) {
          pos = posOuvert;
        }

        porte.write(pos);

        if (pos >= posOuvert) {
          tOpen = millis();
          etat = OUVERT;
        }
      }
      break;

    case OUVERT:
      // attendre 10 secondes avant fermeture
      if (millis() - tOpen >= delaiOpen) {
        etat = FERMETURE;
      }
      break;

    case FERMETURE:
      if (millis() - tMove >= delaiMove) {
        tMove = millis();

        pos -= pasServo;
        if (pos < posFerme) {
          pos = posFerme;
        }

        porte.write(pos);

        if (pos <= posFerme) {
          etat = FERME;
        }
      }
      break;

    case URGENCE:
      // ne rien faire : la porte reste figée
      break;
  }
}

// -------------------- LCD --------------------
void afficherLCD() {
  lcd.setCursor(0, 0);

  if (etat == FERME) {
    lcd.print("Porte fermee    ");
  }
  else if (etat == OUVERTURE) {
    lcd.print("Ouverture...    ");
  }
  else if (etat == OUVERT) {
    lcd.print("Porte ouverte   ");
  }
  else if (etat == FERMETURE) {
    lcd.print("Fermeture...    ");
  }
  else if (etat == URGENCE) {
    lcd.print("!! URGENCE !!   ");
  }

  lcd.setCursor(0, 1);

  if (etat == URGENCE) {
    lcd.print("Moteur arrete   ");
  } else {
    lcd.print("Dist: ");
    lcd.print(dist);
    lcd.print(" cm     ");
  }
}

// -------------------- DEBUG SERIE --------------------
void debugSerie() {
  if (millis() - tPrint >= dPrint) {
    tPrint = millis();

    Serial.print("Dist = ");
    Serial.print(dist);
    Serial.print(" | Etat = ");
    Serial.print(etat);
    Serial.print(" | Pos = ");
    Serial.println(pos);
  }
}