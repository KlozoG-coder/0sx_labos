// Définition de la broche de la DEL
const int ledPin = 13;

// Définition du numéro d'étudiant
const long studentID = 2384980;

// Variables pour gérer l'état de l'application
int appState = 0; // 0 = allumé, 1 = clignotement, 2 = variation d'intensité, 3 = éteint

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); // Initialisation de la communication série pour envoyer les messages
}

void loop() {
  switch (appState) {
    case 0:
      stateOn();
      break;
    case 1:
      stateBlink();
      break;
    case 2:
      stateBrightness();
      break;
    case 3:
      stateOff();
      break;
  }
}

void stateOn() {
  // Allumer la DEL pendant 2 secondes
  digitalWrite(ledPin, HIGH);
  Serial.print("Allumé – ");
  Serial.println(studentID);
  delay(2000);
  appState = 1; // Passer à l'état suivant
}

void stateBlink() {
  // Calcul du nombre de clignotements basé sur l'avant-dernier chiffre de mon numéro d'étudiant
  int secondToLastDigit = (studentID / 10) % 10;
  int blinkCount = (secondToLastDigit == 0) ? 10 : (ceil(secondToLastDigit / 2.0) + 1);

  Serial.print("Clignotement – ");
  Serial.println(studentID);

  for (int i = 0; i < blinkCount; i++) {
    digitalWrite(ledPin, HIGH); // Allumer la DEL
    delay(250);
    digitalWrite(ledPin, LOW); // Éteindre la DEL
    delay(250);
  }

  appState = 2; // Passer à l'état suivant
}

void stateBrightness() {
  // Calcul de la variation d'intensité selon le dernier chiffre du numéro d'étudiant
  int lastDigit = studentID % 10;

  Serial.print("Variation – ");
  Serial.println(studentID);

  if (lastDigit % 2 == 0) {
    for (int i = 0; i <= 255; i++) {
      analogWrite(ledPin, i);
      delay(2048 / 255);
    }
  } else {
    for (int i = 255; i >= 0; i--) {
      analogWrite(ledPin, i);
      delay(2048 / 255);
    }
  }

  appState = 3; // Passer à l'état suivant
}

void stateOff() {
  // Éteindre la DEL pendant 2 secondes
  digitalWrite(ledPin, LOW);
  Serial.print("Éteint – ");
  Serial.println(studentID);
  delay(2000);
  appState = 0; // Retour à l'état initial
}
