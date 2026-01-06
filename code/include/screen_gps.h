/**
 * @file Screen_GPS.h
 * @brief Ekran GPS - interfejs zarządzania modułem GPS
 * @version 1.0
 * @date 2025-01-20
 * 
 * Minimalistyczny ekran z dwoma przyciskami (powrót, reset GPS) oraz miejscem 
 * na logi GPS.
 */

#ifndef SCREEN_GPS_H
#define SCREEN_GPS_H

#include <TFT_eSPI.h>

/**
 * @brief Inicjalizuje ekran GPS
 * 
 * Rysuje przyciski sterujące oraz obszar wyświetlania logów GPS.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void initGpsScreen(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie GPS
 * 
 * Przetwarza naciśnięcia przycisków powrotu i resetu GPS.
 * 
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 */
void handleGpsTouch(uint16_t x, uint16_t y);

#endif // SCREEN_GPS_H