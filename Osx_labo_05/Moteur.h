#ifndef MOTEUR_H
#define MOTEUR_H

#include <Arduino.h>

enum EtatMoteur {
  MOTEUR_ARRET,
  MOTEUR_AVANT,
  MOTEUR_ARRIERE,
  MOTEUR_URGENCE
};

class Moteur {
  private:
    int pinEna;
    int pinIn1;
    int pinIn2;
    int vitesse;
    EtatMoteur etat;

  public:
    Moteur(int ena, int in1, int in2);

    void begin();
    void avancer();
    void reculer();
    void arreter();
    void urgence();

    void setVitesse(int v);
    int getVitesse();

    bool estActif();
    String getSens();
};

#endif