/**
 * @file TFT_Display.h
 * @brief Niskopoziomowa obsługa wyświetlacza TFT
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik zawiera funkcje do konfiguracji wyświetlacza TFT.
 * Odpowiada za inicjalizację wyświetlacza (rotacja, kalibracja)
 * oraz sterowanie jasnością podświetlenia.
 */

#ifndef TFT_DISPLAY_H
#define TFT_DISPLAY_H

#include <TFT_eSPI.h>

/**
 * @brief Inicjalizuje wyświetlacz TFT
 * 
 * Konfiguruje rotację ekranu, kalibrację dotyku oraz
 * ustawienia początkowe wyświetlacza.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 * 
 * @note Należy wywołać przed jakąkolwiek operacją rysowania
 */
void initTFT(TFT_eSPI* tft);

/**
 * @brief Ustawia jasność podświetlenia wyświetlacza
 * 
 * Steruje jasnością przez PWM na pinie backlight.
 * 
 * @param brightness Poziom jasności (0-255)
 *                   0 = podświetlenie wyłączone
 *                   255 = maksymalna jasność
 */
void setBacklight(uint8_t brightness);

#endif // TFT_DISPLAY_H