/**
 * @file Screen_Home.h
 * @brief Główny ekran aplikacji - domyślny widok
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik zawiera funkcje do obsługi ekranu głównego (domyślnego widoku).
 * Odpowiada za wyświetlanie interfejsu użytkownika i obsługę interakcji
 * z przyciskami na ekranie głównym.
 */

#ifndef SCREEN_HOME_H
#define SCREEN_HOME_H

#include <TFT_eSPI.h>

/**
 * @brief Inicjalizuje ekran główny aplikacji
 * 
 * Rysuje główny interfejs z przyciskami menu i widgetami statusu.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void initHomeScreen(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie głównym
 * 
 * Przetwarza naciśnięcia przycisków i uruchamia odpowiednie akcje.
 * 
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 */
void handleHomeTouch(uint16_t x, uint16_t y);

/**
 * @brief Aktualizuje widget statusu GPS na ekranie głównym
 * 
 * Odświeża ikony i informacje o połączeniu GPS bez przerysowywania całego ekranu.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 * 
 * @note Należy wywoływać przy zmianie statusu GPS
 */
void updateGPSStatus(TFT_eSPI* tft);

#endif // SCREEN_HOME_H