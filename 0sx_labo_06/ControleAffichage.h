#ifndef CONTROLEAFFICHAGE_H
#define CONTROLEAFFICHAGE_H

#include <Arduino.h>
#include <LCD_I2C.h>
#include "MatriceDisplay.h"

enum EtatAffichage {
    ETAT_RABAIS,
    ETAT_NORMAL,
    ETAT_ERREUR,
    ETAT_FERMER,
    ETAT_URGENCE
};

class ControleAffichage {

private:
    MatriceDisplay matriceLed;
    LCD_I2C ecranLcd;

    int brocheIr;

public:
    static volatile EtatAffichage etatCourant;

    ControleAffichage(int clkPin, int dinPin, int csPin, int irPin);

    void initialiser(int boutonUrgence);
    void lireCommandes();
    void actualiserAffichage();

    static void gererUrgence();
};

#endif