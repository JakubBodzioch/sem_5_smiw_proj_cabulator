/**
 * @file Screen_OBD.h
 * @brief Ekran OBD - interfejs zarządzania modułem OBD
 * @version 1.0
 * @date 2025-01-20
 * 
 * Minimalistyczny ekran z przyciskami sterującymi i obszarem logów OBD.
 * 
 * @note Komentarz w kodzie mówi "SCREEN GPS" - prawdopodobnie błąd copy-paste
 */

#ifndef SCREEN_OBD_H
#define SCREEN_OBD_H

#include <TFT_eSPI.h>

/**
 * @brief Inicjalizuje ekran OBD
 * 
 * Rysuje przyciski sterujące oraz obszar wyświetlania statusu OBD.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void initObdScreen(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie OBD
 * 
 * Przetwarza naciśnięcia przycisków i nawigację.
 * 
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 */
void handleObdTouch(uint16_t x, uint16_t y);

#endif // SCREEN_OBD_H