/**
 * @file screen_about.h
 * @brief Ekran "About" - informacje o aplikacji
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik zawiera funkcje do inicjalizacji i obsługi ekranu informacyjnego "About".
 */

#ifndef SCREEN_ABOUT_H
#define SCREEN_ABOUT_H

#include <TFT_eSPI.h>

/**
 * @brief Inicjalizuje ekran "About"
 * Rysuje tło i elementy interfejsu na ekranie informacji o aplikacji.
 */
void initAboutScreen(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie "About"
 * 
 * Przetwarza interakcje użytkownika, np. powrót do ekranu głównego.
 * 
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 */
void handleAboutTouch(uint16_t x, uint16_t y);

#endif
