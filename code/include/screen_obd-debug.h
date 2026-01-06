/**
 * @file screen_obd-debug.h
 * @brief Ekran diagnostyki OBD - szczegółowe dane z pojazdu
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik zawiera funkcje do inicjalizacji i aktualizacji ekranu debugowania OBD.
 */

#ifndef SCREEN_OBD_DEBUG_H
#define SCREEN_OBD_DEBUG_H

#include <TFT_eSPI.h>

/**
 * @brief Inicjalizuje ekran debugowania OBD
 * 
 * Rysuje layout z polami na dane diagnostyczne pojazdu.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void initObdDebugScreen(TFT_eSPI* tft);

/**
 * @brief Aktualizuje dane na ekranie debugowania OBD
 * 
 * Odświeża wyświetlane parametry pojazdu (obroty, temperatura, przepływ powietrza).
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 * 
 * @note Należy wywoływać cyklicznie dla aktualnych danych
 */
void updateObdDebugScreen(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie debugowania OBD
 * 
 * Przetwarza interakcje, np. powrót do poprzedniego ekranu.
 * 
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 */
void handleObdDebugTouch(uint16_t x, uint16_t y);

#endif // SCREEN_OBD_DEBUG_H