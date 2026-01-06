/**
 * @file screen_gps-debug.h
 * @brief Ekran diagnostyki GPS - szczegółowe dane satelitarne
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik zawiera funkcje do inicjalizacji i aktualizacji ekranu debugowania GPS.
 */

#ifndef SCREEN_GPS_DEBUG_H
#define SCREEN_GPS_DEBUG_H

#include <TFT_eSPI.h>

/**
 * @brief Inicjalizuje ekran debugowania GPS
 * 
 * Rysuje layout ekranu z polami na dane GPS i status połączenia.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void initGpsDebugScreen(TFT_eSPI* tft);

/**
 * @brief Aktualizuje dane na ekranie debugowania GPS
 * 
 * Odświeża wyświetlane wartości pozycji, liczby satelitów, HDOP
 * oraz inne parametry diagnostyczne.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 * 
 * @note Należy wywoływać cyklicznie dla aktualnych danych
 */
void updateGpsDebugScreen(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie debugowania GPS
 * 
 * Przetwarza interakcje, np. powrót do poprzedniego ekranu.
 * 
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 */
void handleGpsDebugTouch(uint16_t x, uint16_t y);

#endif // SCREEN_GPS_DEBUG_H