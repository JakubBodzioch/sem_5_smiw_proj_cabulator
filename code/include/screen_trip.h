/**
 * @file Screen_Trip.h
 * @brief Ekran trasy - monitorowanie aktywnej podróży
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik nagłówkowy zawiera deklaracje funkcji do obsługi ekranu trasy.
 * Wyświetla przebieg trasy, koszt, zużycie paliwa oraz obsługuje pauzowanie.
 */

#ifndef SCREEN_TRIP_H
#define SCREEN_TRIP_H

#include <TFT_eSPI.h>

/// @name Zmienne stanu trasy
/// @{

/**
 * @brief Przejechany dystans w kilometrach
 * 
 * Wartość akumulowana od rozpoczęcia trasy.
 */
extern float distanceTraveled;

/**
 * @brief Zużyte paliwo w litrach
 * 
 * Suma spalania od rozpoczęcia trasy.
 */
extern float fuelUsed;

/**
 * @brief Czy trasa jest aktywna
 * 
 * true = trip rozpoczęty, false = trip nie rozpoczęty
 */
extern bool tripActive;

/**
 * @brief Czy trasa jest zapauzowana
 * 
 * true = trip wstrzymany, false = trip w trakcie
 */
extern bool tripPaused;

/**
 * @brief Flaga błędu OBD do obsługi w pętli głównej
 * 
 * Sygnalizuje utratę połączenia z modułem OBD podczas trasy.
 */
extern bool obdErrorPending;

/**
 * @brief Flaga resetu logiki tripa
 * 
 * Używana do synchronizacji operacji resetowania w głównej pętli.
 */
extern bool resetTripLogicFlag;

/// @}

/**
 * @brief Inicjalizuje ekran trasy
 * 
 * Rysuje interfejs z danymi trasy (dystans, spalanie, koszt) oraz przyciski sterujące.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void initTripScreen(TFT_eSPI* tft);

/**
 * @brief Aktualizuje wyświetlany status trasy
 * 
 * Odświeża informacje o dystansie, spalaniu i koszcie bez przerysowywania całego ekranu.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 */
void updateTripStatus(TFT_eSPI* tft);

/**
 * @brief Obsługuje dotyk na ekranie trasy
 * 
 * Przetwarza przyciski start/pauza/stop oraz nawigację.
 * 
 * @param x Współrzędna X punktu dotyku
 * @param y Współrzędna Y punktu dotyku
 */
void handleTripTouch(uint16_t x, uint16_t y);

/**
 * @brief Resetuje wszystkie dane trasy do wartości początkowych
 * 
 * Zeruje liczniki dystansu, spalania i kosztu.
 * Wywoływane przy rozpoczynaniu nowej trasy.
 */
void resetTripData();

/**
 * @brief Zapisuje dane trasy do EEPROM
 * 
 * Przechowuje dystans, zużycie paliwa i stan trasy.
 * Wywoływane co 30 sekund podczas aktywnej trasy.
 */
void saveTripDataToEEPROM();

/**
 * @brief Wczytuje dane trasy z EEPROM
 * 
 * Odtwarza poprzednią sesję w przypadku utraty zasilania.
 * @return true jeśli dane zostały pomyślnie wczytane, false jeśli EEPROM jest pusty
 */
bool loadTripDataFromEEPROM();

/**
 * @brief Czyści zapisane dane trasy z EEPROM
 * 
 * Usuwa dane z EEPROM gdy trasa zostanie prawidłowo zakończona.
 */
void clearTripDataFromEEPROM();

#endif // SCREEN_TRIP_H