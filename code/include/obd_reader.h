/**
 * @file OBD_Reader.h
 * @brief Zminimalizowana implementacja czytnika OBDII - komunikacja z ELM327
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik zawiera deklaracje funkcji i zmiennych do obsługi modułu OBDII.
 * Obsługuje inicjalizację, odczyt odometru i spalania oraz obliczanie kosztów.
 * 
 * @see cabulator_settings.h Konfiguracja pinów i parametrów OBD
 * 
 * @namespace OBD
 * @brief Przestrzeń nazw zawierająca całą obsługę modułu OBDII
 */

#pragma once
#include <Arduino.h>
#include "../cabulator_settings.h"

namespace OBD {

    /**
     * @brief Status połączenia Bluetooth z modułem OBD
     * 
     * true jeśli połączenie Bluetooth jest aktywne
     */
    extern bool btConnected;

    /**
     * @brief Status gotowości modułu ELM327
     * 
     * true jeśli moduł odpowiada na komendy AT i jest gotowy do pracy
     */
    extern bool elmReady;

    /**
     * @brief Inicjalizuje i łączy się z modułem OBD przez Bluetooth
     * 
     * Konfiguruje połączenie Bluetooth, wysyła komendy inicjalizacyjne AT
     * i sprawdza komunikację z pojazdem.
     * 
     * @return true jeśli inicjalizacja zakończyła się sukcesem, false w przypadku błędu
     * 
     * @note Wymaga wcześniejszej konfiguracji adresu MAC w cabulator_settings.h
     */
    bool init();

    /**
     * @brief Odczytuje wartość odometru z pojazdu
     * 
     * Wysyła zapytanie PID do ECU i parsuje odpowiedź.
     * 
     * @return Wartość odometru w kilometrach, lub -1 przy błędzie odczytu
     * 
     * @warning Nie wszystkie pojazdy obsługują ten PID
     */
    long readOdometer();

    /**
     * @brief Odczytuje bieżące zużycie paliwa na podstawie MAF
     * 
     * Oblicza spalanie wykorzystując przepływ powietrza (MAF - Mass Air Flow).
     * 
     * @return Spalanie w litrach na godzinę [L/h], lub -1 przy błędzie
     * 
     * @note Dokładność zależy od kalibracji współczynnika AFR
     */
    float readFuelRate();

    /**
     * @brief Oblicza koszt przejazdu na podstawie spalania i taryfy
     * 
     * Wykorzystuje bieżące spalanie i aktywny tryb taryfy do wyliczenia
     * kosztu na jednostkę czasu.
     * 
     * @return Koszt w jednostce walutowej na godzinę
     * 
     * @see readFuelRate() dla źródła danych spalania
     */
    float calculateCost();

    /**
     * @brief Task FreeRTOS obsługujący komunikację OBD w tle
     * 
     * Cyklicznie odpytuje ECU o dane i aktualizuje zmienne globalne.
     * Powinien być uruchomiony przez xTaskCreate().
     * 
     * @param param Parametr przekazywany do tasku (nieużywany)
     * 
     * @note Task działa w nieskończonej pętli z delay
     */
    void task(void* param);

} // namespace OBD