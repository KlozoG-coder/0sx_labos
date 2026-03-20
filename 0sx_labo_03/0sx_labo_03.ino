#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

// Broches
#define ADR 0x27
#define TP A0
#define JX A1
#define JY A2
#define BTN 2
#define LED 8

LiquidCrystal_I2C lcd(ADR, 16, 2);

// Caractère perso
byte c8[8] = {
  B01100,
  B10010,
  B01100,
  B10010,
  B01100,
  B10010,
  B10010,
  B01100
};

// Temps
unsigned long t0 = 0;
unsigned long tSer = 0;
unsigned long tDeb = 0;
unsigned long tAlt = 0;

// Délais
const unsigned long d0 = 3000;
const unsigned long dSer = 100;
const unsigned long dDeb = 50;

// Zone neutre joystick
const int nMin = 450;
const int nMax = 550;

// Seuils température
const float onT = 35.0;
const float offT = 30.0;

// États
bool page2 = false;
bool fan = false;

// Bouton
int lastRead = HIGH;   // dernière lecture brute
int btnState = HIGH;   // état stable validé

// Valeurs
int x = 512;
int y = 512;
int dir = 0;
float alt = 0.0;
float temp = 0.0;
String etat = "STOP";

void setup() {
  Serial.begin(115200);

  pinMode(BTN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, c8);

  t0 = millis();
  tAlt = millis();
}

void loop() {
  unsigned long now = millis();

  // Écran de départ
  if (now - t0 < d0) {
    debut();
    return;
  }

  lireTemp();
  lireJoy();
  calcAlt(now);
  calcDir();
  gererLed();
  gererBtn(now);
  afficher();
  envoyer(now);
}

void debut() {
  lcd.setCursor(0, 0);
  lcd.print("KOUDJONOU       ");

  lcd.setCursor(0, 1);
  lcd.write((uint8_t)0);
  lcd.setCursor(9, 1);
  lcd.print("*****80");
}

void lireTemp() {
  int v = analogRead(TP);
  if (v <= 0) v = 1;

  float r = (1023.0 / v - 1.0) * 10000.0;

  temp = 1.0 / (
           0.001129148 +
           0.000234125 * log(r) +
           0.0000000876741 * pow(log(r), 3)
         ) - 273.15;
}

void lireJoy() {
  x = analogRead(JX);
  y = analogRead(JY);
}

void calcAlt(unsigned long now) {
  float dt = (now - tAlt) / 1000.0;
  tAlt = now;

  if (y > nMax) {
    etat = "UP";
    alt += dt;
  }
  else if (y < nMin) {
    etat = "DOWN";
    alt -= dt;
  }
  else {
    etat = "STOP";
  }

  if (alt < 0) alt = 0;
  if (alt > 200) alt = 200;
}

void calcDir() {
  if (x > nMax) {
    dir = map(x, nMax, 1023, 0, 90);
  }
  else if (x < nMin) {
    dir = map(x, nMin, 0, 0, -90);
  }
  else {
    dir = 0;
  }
}

void gererLed() {
  if (temp > onT) {
    fan = true;
  }
  else if (temp < offT) {
    fan = false;
  }

  digitalWrite(LED, fan ? HIGH : LOW);
}

// Gestion du bouton : 1 clic = 1 changement de page
void gererBtn(unsigned long now) {
  int readBtn = digitalRead(BTN);

  // changement brut -> on relance le chrono anti-rebond
  if (readBtn != lastRead) {
    tDeb = now;
  }

  // si la lecture est stable assez longtemps
  if (now - tDeb > dDeb) {

    // si l’état stable change
    if (readBtn != btnState) {
      btnState = readBtn;

      // INPUT_PULLUP : clic validé quand état stable devient LOW
      if (btnState == LOW) {
        page2 = !page2;
        lcd.clear();
      }
    }
  }

  lastRead = readBtn;
}

void afficher() {
  if (!page2) {
    // Page 1 = vitesse/direction
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(etat);
    lcd.print(" ");
    lcd.print((int)alt);
    lcd.print("m");

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);

    if (dir < 0) {
      lcd.print("G ");
      lcd.print(abs(dir));
      lcd.print("deg");
    }
    else if (dir > 0) {
      lcd.print("D ");
      lcd.print(dir);
      lcd.print("deg");
    }
    else {
      lcd.print("N/A 0deg");
    }
  }
  else {
    // Page 2 = température + état
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temp, 1);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("COOLING:");
    lcd.print(fan ? "ON" : "OFF");
  }
}

void envoyer(unsigned long now) {
  if (now - tSer >= dSer) {
    tSer = now;

    Serial.print("etd:2384980,x:");
    Serial.print(x);
    Serial.print(",y:");
    Serial.print(y);
    Serial.print(",sys:");
    Serial.println(fan ? 1 : 0);
  }
}