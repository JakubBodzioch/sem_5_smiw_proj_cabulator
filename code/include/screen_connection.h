/**
 * @file Screen_Connection.h
 * @brief Ekran połączeń - interfejs konfiguracji połączeń
 * @version 1.0
 * @date 2025-01-20
 * 
 * Minimalistyczny ekran pośredni z dwoma przyciskami i powrotem do ustawień.
 */

#ifndef SCREEN_CONNECTION_H
#define SCREEN_CONNECTION_H

#include <TFT_eSPI.h>

/**
 * @brief Inicjalizuje i wyświetla ekran połączeń
 * 
 * Rysuje przyciski do konfiguracji połączeń oraz opcję powrotu.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void initConnectionScreen(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie połączeń
 * 
 * Przetwarza wybór przycisków i nawigację między ekranami.
 * 
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 */
void handleConnectionTouch(uint16_t x, uint16_t y);

#endif // SCREEN_CONNECTION_H