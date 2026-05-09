#include <Arduino.h>
#include <IRremote.hpp>
#include "ControleAffichage.h"

volatile EtatAffichage ControleAffichage::etatCourant = ETAT_NORMAL;

ControleAffichage::ControleAffichage(int clkPin, int dinPin, int csPin, int irPin)
    : matriceLed(clkPin, dinPin, csPin),
      ecranLcd(0x27, 16, 2),
      brocheIr(irPin) {
}

void ControleAffichage::initialiser(int boutonUrgence) {

    Serial.begin(9600);

    Serial.println("=== LABO 06 PRET ===");
    Serial.println("Commandes disponibles :");
    Serial.println("1 = RABAIS");
    Serial.println("2 = NORMAL");
    Serial.println("3 = ERREUR");
    Serial.println("4 = FERMER");
    Serial.println("Bouton = URGENCE ON/OFF");
    Serial.println("-----------------------");

    matriceLed.begin();

    ecranLcd.begin();
    ecranLcd.backlight();

    IrReceiver.begin(brocheIr);

    pinMode(boutonUrgence, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(boutonUrgence),
        gererUrgence,
        FALLING
    );

    ecranLcd.setCursor(0, 0);
    ecranLcd.print("Labo 06");
}

void ControleAffichage::gererUrgence() {

    static unsigned long dernierAppui = 0;

    unsigned long tempsActuel = millis();

    if (tempsActuel - dernierAppui > 250) {

        if (etatCourant == ETAT_URGENCE) {
            etatCourant = ETAT_NORMAL;
        }
        else {
            etatCourant = ETAT_URGENCE;
        }

        dernierAppui = tempsActuel;
    }
}

void ControleAffichage::lireCommandes() {

    if (etatCourant == ETAT_URGENCE) {

        if (Serial.available()) {
            while (Serial.available()) {
                Serial.read();
            }

            Serial.println("Commande ignoree : mode URGENCE");
        }

        return;
    }

    if (Serial.available()) {

        char commande = Serial.read();

        if (commande == '\n' || commande == '\r') {
            return;
        }

        Serial.print("Commande recue : ");
        Serial.println(commande);

        if (commande == '1') {
            etatCourant = ETAT_RABAIS;
            Serial.println("Mode actif : RABAIS");
        }

        else if (commande == '2') {
            etatCourant = ETAT_NORMAL;
            Serial.println("Mode actif : NORMAL");
        }

        else if (commande == '3') {
            etatCourant = ETAT_ERREUR;
            Serial.println("Mode actif : ERREUR");
        }

        else if (commande == '4') {
            etatCourant = ETAT_FERMER;
            Serial.println("Mode actif : FERMER");
        }

        else {
            Serial.println("Commande inconnue");
            Serial.println("Utilise : 1=Rabais, 2=Normal, 3=Erreur, 4=Fermer");
        }
    }

    if (IrReceiver.decode()) {

        uint32_t codeIr =
            IrReceiver.decodedIRData.decodedRawData;

        Serial.print("Code IR recu : ");
        Serial.println(codeIr, HEX);

        if (codeIr != 0) {

            if (codeIr == 0xF30CFF00) {
                etatCourant = ETAT_RABAIS;
                Serial.println("Mode IR : RABAIS");
            }

            else if (codeIr == 0xE718FF00) {
                etatCourant = ETAT_NORMAL;
                Serial.println("Mode IR : NORMAL");
            }

            else if (codeIr == 0xA15EFF00) {
                etatCourant = ETAT_ERREUR;
                Serial.println("Mode IR : ERREUR");
            }

            else if (codeIr == 0xF708FF00) {
                etatCourant = ETAT_FERMER;
                Serial.println("Mode IR : FERMER");
            }
        }

        IrReceiver.resume();
    }
}

void ControleAffichage::actualiserAffichage() {

    ecranLcd.setCursor(0, 0);

    switch (etatCourant) {

        case ETAT_RABAIS:

            ecranLcd.print("MODE: RABAIS ");
            matriceLed.afficherRabais();

            break;

        case ETAT_NORMAL:

            ecranLcd.print("MODE: NORMAL ");
            matriceLed.afficherNormal();

            break;

        case ETAT_ERREUR:

            ecranLcd.print("MODE: ERREUR ");
            matriceLed.afficherErreur();

            break;

        case ETAT_FERMER:

            ecranLcd.print("MODE: FERMER ");
            matriceLed.eteindre();

            break;

        case ETAT_URGENCE:

            ecranLcd.print("!! URGENCE !!");
            matriceLed.afficherUrgence();

            break;
    }
}