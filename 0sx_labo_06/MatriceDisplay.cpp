#include "MatriceDisplay.h"

// Démarre la communication avec la matrice et règle les paramètres de base (contraste, police).
void MatriceDisplay::begin() {
    u8g2.begin();
    u8g2.setContrast(5);
    u8g2.setFont(u8g2_font_4x6_tr);
}

// Affiche et fait défiler le mot "SPECIAL" horizontalement sur la matrice.
void MatriceDisplay::afficherRabais() {
    u8g2.clearBuffer();
    u8g2.drawStr(xOffset, 7, "SPECIAL");
    u8g2.sendBuffer();
    xOffset--;
    if (xOffset < -30) xOffset = 8;
}

// Dessine un visage souriant (tête, yeux, bouche) sur la matrice 8x8.
void MatriceDisplay::afficherNormal() {
    u8g2.clearBuffer();
    u8g2.drawCircle(3, 3, 3);
    u8g2.drawPixel(2, 2);
    u8g2.drawPixel(4, 2);
    u8g2.drawLine(2, 5, 4, 5);
    u8g2.sendBuffer();
}

// Fait clignoter la matrice entière (carré plein) toutes les 500ms.
void MatriceDisplay::afficherErreur() {
    u8g2.clearBuffer();
    if ((millis() / 500) % 2 == 0) {
        u8g2.drawBox(0, 0, 8, 8);
    }
    u8g2.sendBuffer();
}

// Dessine une croix (X) pour signaler un état critique d'urgence.
void MatriceDisplay::afficherUrgence() {
    u8g2.clearBuffer();
    u8g2.drawLine(0, 0, 7, 7);
    u8g2.drawLine(0, 7, 7, 0);
    u8g2.sendBuffer();
}

// Efface tous les pixels de la matrice pour l'éteindre.
void MatriceDisplay::eteindre() {
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}