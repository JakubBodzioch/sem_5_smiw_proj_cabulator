/**
 * @file Screen_Tariff.h
 * @brief Ekran ustawień taryfy - konfiguracja rozliczeń
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik zawiera funkcje do obsługi ekranu taryfy.
 * Umożliwia użytkownikowi ustawienie wartości taryfy za km lub litr
 * oraz zarządzanie trybem rozliczeń.
 */

#ifndef SCREEN_TARIFF_H
#define SCREEN_TARIFF_H

#include <TFT_eSPI.h>

/**
 * @enum TariffMode
 * @brief Tryby obliczania taryfy
 * 
 * Określa sposób rozliczania kosztów przejazdu.
 */
enum TariffMode {
    TARIFF_PER_KM = 0,      ///< Stała stawka za kilometr
    TARIFF_PER_LITRE = 1    ///< Stała stawka za spalony litr paliwa
};

/**
 * @brief Wartość taryfy w aktualnym trybie
 * 
 * Przechowuje cenę za jednostkę (km lub litr) w zależności od trybu.
 */
extern float tariffValue;

/**
 * @brief Aktualny tryb taryfy
 * 
 * Określa czy rozliczamy za km czy za litr paliwa.
 */
extern TariffMode tariffMode;

/**
 * @brief Inicjalizuje ekran konfiguracji taryfy
 * 
 * Rysuje kontrolki do zmiany wartości i trybu taryfy.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void initTariffScreen(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie taryfy
 * 
 * Przetwarza zmianę wartości, przełączanie trybów i zapis ustawień.
 * 
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 */
void handleTariffTouch(uint16_t x, uint16_t y);

/**
 * @brief Rysuje aktualną wartość taryfy na ekranie
 * 
 * Odświeża wyświetlaną liczbę po zmianie wartości przez użytkownika.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void drawTariffValue(TFT_eSPI* tft);

/**
 * @brief Ładuje ustawienia taryfy z EEPROM
 * 
 * Odczytuje zapisaną wartość i tryb taryfy z pamięci nieulotnej.
 * Wywoływane przy starcie aplikacji.
 */
void loadTariffFromEEPROM();

/**
 * @brief Zapisuje ustawienia taryfy do EEPROM
 * 
 * Zachowuje aktualną wartość i tryb taryfy w pamięci nieulotnej.
 */
void saveTariffToEEPROM();

#endif // SCREEN_TARIFF_H