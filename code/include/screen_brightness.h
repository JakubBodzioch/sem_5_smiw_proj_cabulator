/**
 * @file Screen_Brightness.h
 * @brief Ustawienia jasności ekranu - kontrola podświetlenia TFT
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik zawiera funkcje i typy do obsługi ekranu jasności.
 * Pozwala na inicjalizację modułu, obsługę dotyku oraz zmianę poziomu jasności
 * podświetlenia wyświetlacza.
 */

#ifndef SCREEN_BRIGHTNESS_H
#define SCREEN_BRIGHTNESS_H

#include <TFT_eSPI.h>
#include <Arduino.h>

/**
 * @brief Aktualny poziom jasności podświetlenia
 * 
 * Wartość w zakresie 0-100 określająca procent maksymalnej jasności.
 * - 0 = podświetlenie wyłączone
 * - 100 = maksymalna jasność
 */
extern uint8_t brightnessLevel;

/**
 * @brief Inicjalizuje moduł sterowania jasnością
 * 
 * Konfiguruje PWM do kontroli podświetlenia i wczytuje zapisaną wartość z EEPROM.
 * Należy wywołać raz podczas startu aplikacji.
 */
void initBrightnessModule();

/**
 * @brief Inicjalizuje ekran ustawień jasności
 * 
 * Rysuje interfejs z suwakiem jasności i przyciskami kontrolnymi.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void initBrightnessScreen(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie ustawień jasności
 * 
 * Przetwarza interakcje z suwakiem i przyciskami, aktualizuje jasność w czasie rzeczywistym.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 * @param[out] levelOut Wskaźnik do zmiennej gdzie zostanie zapisany nowy poziom jasności
 */
void handleBrightnessTouch(TFT_eSPI* tft, uint16_t x, uint16_t y, uint8_t* levelOut);

#endif // SCREEN_BRIGHTNESS_H