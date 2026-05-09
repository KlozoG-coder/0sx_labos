#ifndef MATRICEDISPLAY_H
#define MATRICEDISPLAY_H

#include <U8g2lib.h>

class MatriceDisplay {
private:
    U8G2_MAX7219_8X8_F_4W_SW_SPI u8g2;
    
    // Variable pour mémoriser la position horizontale du texte lors de l'animation de défilement.
    int xOffset = 8;

public:
    // Constructeur : initialise l'objet 
    MatriceDisplay(int clk, int din, int cs) 
        : u8g2(U8G2_R0, clk, din, cs, U8X8_PIN_NONE, U8X8_PIN_NONE) {}
    void begin();
    void afficherRabais();
    void afficherNormal();
    void afficherErreur();
    void afficherUrgence();
    void eteindre();
};

#endif