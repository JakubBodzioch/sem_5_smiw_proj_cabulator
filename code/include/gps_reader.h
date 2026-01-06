/**
 * @file GPS_Reader.h
 * @brief Obsługa modułu GPS - pobieranie pozycji i danych satelitarnych
 * @version 1.0
 * @date 2025-01-20
 *
 * Plik zawiera funkcje i struktury do obsługi GPS.
 * Pozwala na inicjalizację, pobieranie danych, sprawdzanie statusu
 * oraz debugowanie połączenia z GPS.
 *
 * @see cabulator_settings.h Konfiguracja pinów i parametrów GPS
 */

#ifndef GPS_READER_H
#define GPS_READER_H

#include <Arduino.h>
#include "../cabulator_settings.h"

/**
 * @namespace GPS
 * @brief Przestrzeń nazw zawierająca całą obsługę modułu GPS
 *
 * Zapewnia interfejs do komunikacji z modułem GPS przez UART,
 * parsowanie danych NMEA oraz udostępnianie aktualnej pozycji.
 */
namespace GPS {

    /**
     * @struct Fix
     * @brief Struktura przechowująca dane z odczytu GPS (fix)
     *
     * Zawiera wszystkie istotne informacje o aktualnej pozycji
     * oraz metadane o jakości sygnału.
     *
     * @note Pole `valid` należy sprawdzić przed użyciem współrzędnych
     */
    struct Fix {
        bool valid;             ///< Czy pozycja jest ważna (fix uzyskany)
        uint32_t takenAtMs;     ///< Timestamp pobrania próbki [ms od startu]
        uint8_t sats;           ///< Liczba widocznych satelitów
        uint16_t hdop;          ///< Precyzja pozioma HDOP (x100, np. 120 = 1.20)
        double lat;             ///< Szerokość geograficzna [stopnie]
        double lng;             ///< Długość geograficzna [stopnie]
        
        uint16_t year;          ///< Rok (np. 2025)
        uint8_t month;          ///< Miesiąc (1-12)
        uint8_t day;            ///< Dzień (1-31)
        uint8_t hour;           ///< Godzina (0-23)
        uint8_t minute;         ///< Minuta (0-59)
        uint8_t second;         ///< Sekunda (0-59)
        bool dateTimeValid;     ///< Czy data/czas są ważne
    };

    /**
     * @brief Inicjalizacja modułu GPS
     *
     * Konfiguruje UART i rozpoczyna nasłuchiwanie danych NMEA.
     * Należy wywołać raz w setup().
     *
     * @note Piny i baudrate konfigurowane w cabulator_settings.h
     */
    void begin();

    /**
     * @brief Pobiera najnowszą próbkę GPS
     * @param[out] out Referencja do struktury Fix, która zostanie wypełniona danymi
     * @return true jeśli udało się pobrać nowe dane, false jeśli brak nowych danych
     *
     * @warning Sprawdź pole `out.valid` - może być false nawet przy return true
     */
    bool poll(Fix& out);

    /**
     * @brief Sprawdza, czy mamy aktualny fix GPS
     * @return true jeśli ostatni fix jest ważny i aktualny, false w przeciwnym razie
     *
     * Przydatne do szybkiego sprawdzenia stanu GPS bez pobierania pełnych danych.
     */
    bool hasLiveFix();

    /**
     * @brief Wypisuje status GPS na Serial (debug)
     *
     * Drukuje informacje o:
     * - Statusie połączenia
     * - Liczbie satelitów
     * - Ostatniej pozycji
     * - Wartości HDOP
     */
    void debugStatus();

    /**
     * @brief Zwraca surowe linie NMEA z modułu GPS
     * @return Wskaźnik do tablicy wskaźników na C-stringi z liniami NMEA
     *
     * Przydatne do debugowania i analizy surowych danych z GPS.
     *
     * @note Dane są nadpisywane przy każdym wywołaniu poll()
     */
    /**
     * @brief Ustaw systemowy zegar ESP32 na podstawie danych GPS
     * @param fix Struktura Fix zawierająca dane daty/czasu z GPS
     * @return true jeśli udało się ustawić zegar, false jeśli data/czas były nieważne
     *
     * Synchronizuje wewnętrzny zegar ESP32 z czasem z modułu GPS.
     * Powinno być wywoływane gdy fix.dateTimeValid == true
     */
    bool setSystemTimeFromGPS(const Fix& fix);

    const char* const* getLastRawLines();

    /**
     * @brief Ostatnia pobrana próbka GPS
     *
     * Globalna zmienna przechowująca ostatni odczyt.
     * Aktualizowana automatycznie przez poll().
     *
     * @see poll() do pobrania świeżych danych
     */
    extern Fix lastFix;

}

#endif // GPS_READER_H