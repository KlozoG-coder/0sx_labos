#include "ControleAffichage.h"

ControleAffichage controle(6, 4, 5, 11);

void setup() {
    controle.initialiser(3);
}

void loop() {
    controle.lireCommandes();
    controle.actualiserAffichage();
    delay(50);
}