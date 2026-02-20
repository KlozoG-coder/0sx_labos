// --- Nom : KOUDJONOU K. Jean Louis ---
// --- Numero Etudiant : 2384980 ---
unsigned long currentTime = 0;

const int NB_LEDS = 4;
const int ledPins[NB_LEDS] = {7, 8, 9, 10};   
const int buttonPin = 2;
const int potentiometerPin = A1;

// --- Variables potentiomètre ---
int potValue = 0;
int scaleValue = 0;       // 0 à 20
int percentValue = 0;

// --- Anti-rebond ---
int stableButtonState = HIGH;
int lastReading = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// --- Constante barre ---
const int BAR_MAX = 20;

void setup() {

  Serial.begin(9600);

  for (int i = 0; i < NB_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {

  currentTime = millis();

  readPotentiometer();
  updateLedsRealtime();
  handleButton();
}

// Lecture et conversion

void readPotentiometer() {

  potValue = analogRead(potentiometerPin);

  // Conversion 0-1023 - 0-20
  scaleValue = map(potValue, 0, 1023, 0, BAR_MAX);

  percentValue = (scaleValue * 100) / BAR_MAX;
}

void updateLedsRealtime() {

  int ledIndex = percentValue / 25;   // 0-3

  if (ledIndex > 3) ledIndex = 3;

  for (int i = 0; i < NB_LEDS; i++) {
    digitalWrite(ledPins[i], (i == ledIndex) ? HIGH : LOW);
  }
}

// Gestion bouton avec debounce
void handleButton() {

  int reading = digitalRead(buttonPin);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != stableButtonState) {

      stableButtonState = reading;

      if (stableButtonState == LOW) {
        printProgressBar();
      }
    }
  }

  lastReading = reading;
}

void printProgressBar() {

  Serial.print("[");

  for (int i = 0; i < BAR_MAX; i++) {
    if (i < scaleValue) {
      Serial.print("!");
    } else {
      Serial.print(".");
    }
  }

  Serial.print("] ");
  Serial.print(percentValue);
  Serial.println("%");
}
