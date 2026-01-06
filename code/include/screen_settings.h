/**
 * @file Screen_Settings.h
 * @brief Ekran ustawień aplikacji
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik zawiera funkcje do obsługi ekranu ustawień.
 * Umożliwia użytkownikowi dostęp do konfiguracji taryfy, podświetlenia
 * oraz ustawień połączenia z siecią.
 */

#ifndef SCREEN_SETTINGS_H
#define SCREEN_SETTINGS_H

#include <TFT_eSPI.h>

/**
 * @brief Inicjalizuje ekran ustawień
 * 
 * Rysuje menu z opcjami konfiguracji taryfy, jasności i połączeń.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void initSettingsScreen(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie ustawień
 * 
 * Przetwarza wybór opcji i przejścia do ekranów szczegółowych ustawień.
 * 
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 */
void handleSettingsTouch(uint16_t x, uint16_t y);

#endif // SCREEN_SETTINGS_H