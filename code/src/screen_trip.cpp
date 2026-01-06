#include "screen_trip.h"
#include "screen_obd.h"
#include "tft_display.h"
#include "background.h"
#include "gui_elements.h"
#include "obd_reader.h"
#include "gps_reader.h"
#include "screen_tariff.h"
#include "screen_manager.h"
#include "screen_home.h"
#include "sd_manager.h"

#include <Arduino.h>
#include <EEPROM.h>
#include "esp_task_wdt.h"

// EEPROM adresy dla danych trasy (po tariff: bajt 15+)
#define TRIP_EEPROM_VALID_FLAG 15    // 1 bajt - flaga ważności danych
#define TRIP_EEPROM_DISTANCE 16      // 4 bajty - dystans (float)
#define TRIP_EEPROM_FUEL 20          // 4 bajty - paliwo (float)
#define TRIP_EEPROM_PAUSED 24        // 1 bajt - flaga pauzowania

// Bufory tekstów do aktualizacji tylko przy zmianie
bool tripActive = false;
bool tripPaused = false;
static char lastDueText[32] = "";
static char lastDistText[32] = "";
static char lastFuelText[32] = "";
static TFT_eSPI* tftPtr = nullptr;
static Background* bgTrip = nullptr;

// Dane trasy (globalne, liczone w tle)
float distanceTraveled = 0.0f; // w km
float fuelUsed = 0.0f;         // w litrach
bool obdErrorPending = false;  // Flaga błędu OBD do obsługi w pętli głównej
bool resetTripLogicFlag = false;  // Flaga resetu logiki tripa

// Zapisanie danych trasy do EEPROM
void saveTripDataToEEPROM() {

    // Flaga ważności - 0xAB oznacza że dane są ważne
    EEPROM.write(TRIP_EEPROM_VALID_FLAG, 0xAB);
    
    // Zapisz dystans (float = 4 bajty)
    EEPROM.writeFloat(TRIP_EEPROM_DISTANCE, distanceTraveled);
    
    // Zapisz zużycie paliwa (float = 4 bajty)
    EEPROM.writeFloat(TRIP_EEPROM_FUEL, fuelUsed);
    
    // Zapisz stan pauzy (1 bajt)
    EEPROM.write(TRIP_EEPROM_PAUSED, tripPaused ? 1 : 0);
    
    // Zatwierdź zapisy
    EEPROM.commit();
    
    Serial.printf("[TRIP] Saved to EEPROM: dist=%.2f km, fuel=%.2f L, paused=%d\n", distanceTraveled, fuelUsed, tripPaused);
}

// Wczytanie danych trasy z EEPROM
bool loadTripDataFromEEPROM() {

    // Sprawdź flagę ważności
    uint8_t validFlag = EEPROM.read(TRIP_EEPROM_VALID_FLAG);

    if (validFlag != 0xAB) {

        Serial.println("[TRIP] No valid trip data in EEPROM");
        return false;
    }
    
    // Wczytaj dane
    distanceTraveled = EEPROM.readFloat(TRIP_EEPROM_DISTANCE);
    fuelUsed = EEPROM.readFloat(TRIP_EEPROM_FUEL);
    tripPaused = (EEPROM.read(TRIP_EEPROM_PAUSED) == 1);
    
    // Wczytaj ścieżkę SD z EEPROM
    extern String currentTripPath;
    currentTripPath = SDManager::getLastTripPath();
    
    Serial.printf("[TRIP] Loaded from EEPROM: dist=%.2f km, fuel=%.2f L, paused=%d\n", distanceTraveled, fuelUsed, tripPaused);
    
    return true;
}

// Czyszczenie danych trasy z EEPROM
void clearTripDataFromEEPROM() {

    EEPROM.write(TRIP_EEPROM_VALID_FLAG, 0xFF); // Flaga nieważności
    SDManager::clearLastTripPath(); // Wyczyść ścieżkę SD
    EEPROM.commit();
    Serial.println("[TRIP] Trip data cleared from EEPROM");
}

// Reset danych trasy
void resetTripData() {

    distanceTraveled = 0.0f;
    fuelUsed = 0.0f;
    tripPaused = false;
    tripActive = false;
    resetTripLogicFlag = true;  // Zresetuj także logikę liczenia
    resetTextBuffer(lastDueText, sizeof(lastDueText));
    resetTextBuffer(lastDistText, sizeof(lastDistText));
    resetTextBuffer(lastFuelText, sizeof(lastFuelText));
}

// Inicjalizacja ekranu trasy
void initTripScreen(TFT_eSPI* tft) {

    tftPtr = tft;
    if (bgTrip) { delete bgTrip; bgTrip = nullptr; }
    
    // Próba wczytania poprzedniej sesji z EEPROM
    if (!loadTripDataFromEEPROM()) {
        
        // Jeśli nie ma danych w EEPROM, RESET
        distanceTraveled = 0.0f;
        fuelUsed = 0.0f;
        tripPaused = false;
    }
    
    // Wznowienie istniejącego stanu tripa (nie resetuj)
    if (tripPaused)
        bgTrip = new Background("/trip_paused.png");
    else
        bgTrip = new Background("/trip.png");

    // Rysowanie tła
    bgTrip->draw(*tft, *bgTrip->s_png, true);
    tripActive = true;
    Serial.println("[TRIP] Trip screen initialized - STARTING TRIP");
    Serial.printf("[TRIP] OBD::btConnected=%d, OBD::elmReady=%d\n", OBD::btConnected, OBD::elmReady);
    

    // ========== Tworzenie nowej sesji SD ==========
    // Tylko tworzy nową sesję jeśli trip nie był aktywny (nie było ścieżki)
    if (SDManager::isReady()) {

        extern String currentTripPath;
        
        // Jeśli currentTripPath jest pusta, to znaczy że to nowy trip
        if (currentTripPath.isEmpty()) {

            
            currentTripPath = SDManager::createTripSession();

            if (!currentTripPath.isEmpty())
                Serial.printf("[TRIP] SD session created: %s\n", currentTripPath.c_str());

        } else {

            // Trip już był aktywny, wznowienie istniejącej sesji
            Serial.printf("[TRIP] Resuming existing SD session: %s\n", currentTripPath.c_str());
        }
    }
    
    // Reset buforów tekstów do aktualizacji
    resetTextBuffer(lastDueText, sizeof(lastDueText));
    resetTextBuffer(lastDistText, sizeof(lastDistText));
    resetTextBuffer(lastFuelText, sizeof(lastFuelText));
    updateTripStatus(tftPtr);
}

// Aktualizacja statusu trasy
void updateTripStatus(TFT_eSPI* tft) {

    if (!tft) return;
    // Obliczenie należności
    float currentDue = 0.0f;
    int startedKm = (int)distanceTraveled + 1; // Każdy rozpoczęty km, minimum 1

    if (tariffMode == TARIFF_PER_KM)
        currentDue = startedKm * tariffValue;
    else
        currentDue = fuelUsed * tariffValue;
        
    // Bufory tekstów
    char dueBuf[32], distBuf[32], fuelBuf[32];
    sprintf(dueBuf, "%.2f ZL", currentDue);
    sprintf(distBuf, "%d km", startedKm);
    sprintf(fuelBuf, "%.2f L", fuelUsed);
    
    // Rysowanie tekstu tylko jeśli zmienne zmieniły wartość od ostatniego rysowania
    if (strcmp(dueBuf, lastDueText) != 0) {

        drawTextWithBackground(tft, dueBuf, 300, 70, TR_DATUM, 2, TFT_YELLOW, TFT_BLACK, 140);
        strcpy(lastDueText, dueBuf);
    }

    if (strcmp(distBuf, lastDistText) != 0) {

        drawTextWithBackground(tft, distBuf, 300, 110, TR_DATUM, 2, TFT_GREEN, TFT_BLACK, 120);
        strcpy(lastDistText, distBuf);
    }

    if (strcmp(fuelBuf, lastFuelText) != 0) {

        drawTextWithBackground(tft, fuelBuf, 300, 130, TR_DATUM, 2, TFT_CYAN, TFT_BLACK, 120);
        strcpy(lastFuelText, fuelBuf);
    }
}

// Obsługa dotyku na ekranie trasy
void handleTripTouch(uint16_t x, uint16_t y) {

    // Przycisk pauzy/odpauzowania
    if (x >= 20 && x < 220 && y >= 200 && y < 240) {

        if (!tripPaused) {

            tripPaused = true;
            if (bgTrip) { delete bgTrip; bgTrip = nullptr; }
            bgTrip = new Background("/trip_paused.png");
            bgTrip->draw(*tftPtr, *bgTrip->s_png, true);
            resetTextBuffer(lastDueText, sizeof(lastDueText));
            resetTextBuffer(lastDistText, sizeof(lastDistText));
            resetTextBuffer(lastFuelText, sizeof(lastFuelText));
            updateTripStatus(tftPtr);
            Serial.println("[TRIP] Trip paused");
            delay(50);

        } else {

            tripPaused = false;
            if (bgTrip) { delete bgTrip; bgTrip = nullptr; }
            bgTrip = new Background("/trip.png");
            bgTrip->draw(*tftPtr, *bgTrip->s_png, true);
            resetTextBuffer(lastDueText, sizeof(lastDueText));
            resetTextBuffer(lastDistText, sizeof(lastDistText));
            resetTextBuffer(lastFuelText, sizeof(lastFuelText));
            updateTripStatus(tftPtr);
            Serial.println("[TRIP] Trip resumed");
            delay(50);
        }
        return;
    }

    // Przycisk powrotu: jeśli trip jest zapauzowany, kasuje tripa, jeśli nie to tylko wraca do ekranu głównego
    if (x >= 260 && x < 310 && y >= 10 && y < 50) {

        if (tripPaused) {

            // ========== FINALIZACJA TRASY NA KARCIE SD ==========
            // Trip jest finalizowany TYLKO jeśli był wznowiony (pauzowany)
            extern String currentTripPath;
            if (SDManager::isReady() && !currentTripPath.isEmpty()) {

                SDManager::TripData finalData;
                finalData.distanceKm = distanceTraveled;
                finalData.fuelUsedLiters = fuelUsed;
                finalData.tariffMode = tariffMode;
                finalData.tariffValue = tariffValue;
                
                // Wiliczenie całkowietego kosztu
                int startedKm = (int)distanceTraveled + 1;
                if (tariffMode == TARIFF_PER_KM)
                    finalData.totalCost = startedKm * tariffValue;
                else
                    finalData.totalCost = fuelUsed * tariffValue;
                
                SDManager::finalizeTrip(finalData);
                currentTripPath = "";  // Wyczyszczenie ścieżki bieżącej trasy
            }
            
            resetTripData();
            clearTripDataFromEEPROM();
            Serial.println("[TRIP] Trip ended and reset (exit from pause)");
        } else {
            // Trip nie jest wznowiony (nadal aktywny) - wracamy do home bez finalizacji
            // tripActive zostaje true, aby można było wznowić trip gdy wrócimy
            Serial.println("[TRIP] Returning to home - trip remains active (can be resumed)");
        }


        // Powrót do ekranu głównego
        initHomeScreen(tftPtr);
        currentScreen = SCREEN_HOME;
        tftPtr = nullptr;
        return;
    }
}