/**
 * @file sd_manager.h
 * @brief Obsługa karty SD - logowanie tras i danych GPS
 * @version 1.0
 * @date 2025-01-20
 *
 * @details
 * Plik zawiera funkcje do obsługi karty SD, tworzenia folderów tras
 * i zapisywania danych z przejazdu.
 * 
 * ## Architektura SPI
 * 
 * Projekt używa **dwóch niezależnych bus SPI** aby uniknąć konfliktów między urządzeniami:
 * 
 * ### VSP (Primary SPI) - dla wyświetlacza TFT i touch input
 * Obsługiwany przez bibliotekę TFT_eSPI:
 * - **CLK**  = GPIO18 (Serial Clock)
 * - **MOSI** = GPIO23 (Master Out, Slave In - dane do TFT)
 * - **MISO** = GPIO19 (Master In, Slave Out - dane z touch)
 * 
 * ### HSPI (Secondary SPI) - dla karty SD
 * Inicjalizowany przez SDManager::init() w sd_manager.cpp:
 * - **CLK**  = GPIO27 (Serial Clock)
 * - **MOSI** = GPIO14 (Master Out, Slave In - dane do karty SD)
 * - **MISO** = GPIO12 (Master In, Slave Out - dane z karty SD)
 * - **CS**   = GPIO13 (Chip Select - aktywacja karty SD)
 * 
 * **Zaleta:** Dzięki oddzielnym bus SPI, wyświetlacz i karta SD mogą pracować równocześnie
 * bez konfliktów, ponieważ każde urządzenie ma swoje dedykowane linie komunikacyjne.
 * 
 * ## Struktura danych
 * 
 * Każda trasa jest przechowywana w strukturze:
 * ```
 * /logs/trips/YYYY-MM-DD_HH-MM-SS/
 * gps_log.csv        (dane GPS, ~1 entry/sek)
 * trip_data.csv      (dane końcowe trasy)
 * ```
 * 
 * ## Przepływ danych
 * 
 * - GPS podaje nowy fix → SDManager::onGPSFix() zapisuje do gps_log.csv
 * - Użytkownik kończy trasę → SDManager::finalizeTrip() zapisuje trip_data.csv
 */

#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#include <Arduino.h>
#include <SD.h>
#include <FS.h>

namespace SDManager {

    /**
     * @struct TripData
     * @brief Struktura danych trasy do zapisu
     */
    struct TripData {
        float distanceKm;           ///< Przejechany dystans w km
        float fuelUsedLiters;       ///< Zużyte paliwo w litrach
        int tariffMode;             ///< Tryb taryfy (0=km, 1=paliwo)
        float tariffValue;          ///< Wartość taryfy
        float totalCost;            ///< Całkowity koszt
    };

    /**
     * @struct TripUpdateData
     * @brief Struktura danych aktualizacji trasy do zapisu co 10 sekund
     */
    struct TripUpdateData {
        float distanceKm;           ///< Przejechany dystans w km (bieżący)
        float fuelUsedLiters;       ///< Zużyte paliwo w litrach (bieżące)
        float totalCost;            ///< Całkowity koszt (bieżący)
        unsigned long timestamp;    ///< Timestamp w ms od startu
    };

    /**
     * @struct GPSData
     * @brief Struktura danych GPS do zapisu
     */
    struct GPSData {
        double latitude;            ///< Szerokość geograficzna
        double longitude;           ///< Długość geograficzna
        uint8_t satellites;         ///< Liczba satelitów
        uint16_t hdop;              ///< Precyzja HDOP
        bool valid;                 ///< Czy fix jest ważny
        unsigned long timestamp;    ///< Timestamp w ms od startu
    };

    /**
     * @brief Inicjalizuje kartę SD na HSPI (Secondary SPI)
     *
     * @details
     * Funkcja konfiguruje HSPI bus (Secondary SPI)
     * 
     * Używanie osobnego bus (HSPI) zamiast domyślnego SPI (VSP) pozwala
     * karcie SD pracować równolegle z TFT (wyświetlacz) i touch inputem,
     * bez konfliktów na linii komunikacyjnej.
     * 
     * Funkcja tworzy również strukturę katalogów /logs/trips/ na karcie SD.
     *
     * @return true jeśli SD została poprawnie zainicjalizowana i zmontowana, 
     *         false w przypadku błędu (np. brak karty, błąd zapisu)
     * 
     * @note Powinna być wywołana w setup() po inicjalizacji TFT, aby upewnić się
     *       że touch będzie poprawnie skalibrowany po incjalizacji SD.
     * 
     * @see SDManager::onGPSFix(), SDManager::finalizeTrip()
     */
    bool init();

    /**
     * @brief Sprawdza czy karta SD jest gotowa
     *
     * @return true jeśli karta SD jest dostępna
     */
    bool isReady();

    /**
     * @brief Wczytuje ostatnią ścieżkę trasy z EEPROM
     *
     * @return Ścieżka trasy zapisana w EEPROM, lub pusty string jeśli nic nie zapisane
     */
    String getLastTripPath();

    /**
     * @brief Czyści ostatnią ścieżkę trasy z EEPROM
     */
    void clearLastTripPath();

    /**
     * @brief Tworzy nową sesję trasy z timestamp'em
     *
     * Tworzy folder w formacie "YYYY-MM-DD_HH-MM-SS" w katalogu /logs/trips/
     * @return Ścieżka utworzonego folderu, lub pusty string jeśli błąd
     */
    String createTripSession();

    /**
     * @brief Zapisuje dane trasy do pliku trip_data.csv
     *
     * @param tripPath Ścieżka folderu trasy (zwrócona z createTripSession)
     * @param data Struktura z danymi trasy
     * @return true jeśli zapis się powiódł, false w przypadku błędu
     */
    bool saveTripData(const String& tripPath, const TripData& data);

    /**
     * @brief Zapisuje wpis danych GPS do pliku gps_log.csv
     *
     * @param tripPath Ścieżka folderu trasy
     * @param data Struktura z danymi GPS
     * @return true jeśli zapis się powiódł, false w przypadku błędu
     */
    bool saveGPSData(const String& tripPath, const GPSData& data);

    /**
     * @brief Callback wywoływany przez GPS gdy pojawi się nowy fix
     *
     * @details
     * Ta funkcja jest automatycznie wywoływana przez moduł GPS (z gps_reader.cpp)
     * każdorazowo gdy nowy fix jest dostępny (~co 1 sekundę podczas normalnej pracy).
     * 
     * Jeśli trip jest aktywny (currentTripPath != ""), dane GPS są zapisywane 
     * do pliku gps_log.csv w folderze bieżącej trasy.
     * 
     * Zapis odbywa się na HSPI bus, więc nie blokuje operacji na TFT/touch.
     *
     * @param data Struktura GPSData zawierająca: szerokość, długość, satelity, HDOP, valid, timestamp
     * 
     * @note Ta funkcja jest non-blocking i powinna być szybka (wpis do CSV)
     * @see gps_reader.cpp - tam gdzie jest wywoływana
     * @see GPSData - struktura danych GPS
     */
    void onGPSFix(const GPSData& data);

    /**
     * @brief Callback wywoływany przez OBD co ~10 sekund - aktualizacja danych tripu
     *
     * @details
     * Ta funkcja jest wywoływana przez OBD::task() co ~10 sekund z bieżącymi danymi tripu.
     * Zapisuje dystans, spalanie i koszt do pliku obd_log.csv w folderze bieżącej trasy.
     * 
     * Zapis odbywa się na HSPI bus, więc nie blokuje operacji na TFT/touch.
     *
     * @param data Struktura TripUpdateData zawierająca: dystans, spalanie, koszt, timestamp
     * 
     * @note Ta funkcja jest non-blocking i powinna być szybka (wpis do CSV)
     * @see obd_reader.cpp - tam gdzie jest wywoływana
     * @see TripUpdateData - struktura danych tripu
     */
    void onTripUpdate(const TripUpdateData& data);

    /**
     * @brief Finalizuje trasę - zapisuje ostateczne dane podsumowania na SD
     *
     * @details
     * Ta funkcja powinna być wywoływana gdy użytkownik kończy trasę
     * (np. wciśnie przycisk "Stop" w screen_trip.cpp).
     * 
     * Funkcja zapisuje dane końcowe trasy (distanceKm, fuelUsedLiters, cost, etc.)
     * do pliku trip_summary.csv w folderze bieżącej trasy.
     * 
     * Po zapisaniu, zmienną globalną currentTripPath powinna być wyczyszczona.
     *
     * @param data Struktura TripData zawierająca podsumowanie przejazdu
     * 
     * @note Po wyeliminowaniu tej funkcji trip jest "zamknięty" na SD
     * @see screen_trip.cpp - tam gdzie jest wywoływana
     * @see TripData - struktura danych trasy
     */
    void finalizeTrip(const TripData& data);

    /**
     * @brief Listuje wszystkie dostępne logi tras
     *
     * Wypisuje na Serial listę folderów z trasami.
     */
    void listTrips();

    /**
     * @brief Pobiera dane z ostatniej trasy
     *
     * @param tripPath[out] Ścieżka do folderu ostatniej trasy
     * @return true jeśli znaleziono trasę, false jeśli brak tras
     */
    bool getLastTrip(String& tripPath);

}  // namespace SDManager

#endif  // SD_MANAGER_H
