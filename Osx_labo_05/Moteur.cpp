#include "Moteur.h"

Moteur::Moteur(int ena, int in1, int in2) {
  pinEna = ena;
  pinIn1 = in1;
  pinIn2 = in2;
  vitesse = 0;
  etat = MOTEUR_ARRET;
}

void Moteur::begin() {
  pinMode(pinEna, OUTPUT);
  pinMode(pinIn1, OUTPUT);
  pinMode(pinIn2, OUTPUT);
  arreter();
}

void Moteur::avancer() {
  etat = MOTEUR_AVANT;

  digitalWrite(pinIn1, HIGH);
  digitalWrite(pinIn2, LOW);
  analogWrite(pinEna, vitesse);
}

void Moteur::reculer() {
  etat = MOTEUR_ARRIERE;

  digitalWrite(pinIn1, LOW);
  digitalWrite(pinIn2, HIGH);
  analogWrite(pinEna, vitesse);
}

void Moteur::arreter() {
  etat = MOTEUR_ARRET;

  digitalWrite(pinIn1, LOW);
  digitalWrite(pinIn2, LOW);
  analogWrite(pinEna, 0);
}

void Moteur::urgence() {
  etat = MOTEUR_URGENCE;

  digitalWrite(pinIn1, LOW);
  digitalWrite(pinIn2, LOW);
  analogWrite(pinEna, 0);
}

void Moteur::setVitesse(int v) {
  vitesse = constrain(v, 0, 255);
}

int Moteur::getVitesse() {
  return vitesse;
}

bool Moteur::estActif() {
  return etat == MOTEUR_AVANT || etat == MOTEUR_ARRIERE;
}

String Moteur::getSens() {
  if (etat == MOTEUR_AVANT) {
    return "AVANT";
  }

  if (etat == MOTEUR_ARRIERE) {
    return "ARRIERE";
  }

  return "ARRET";
}