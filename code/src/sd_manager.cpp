#include "sd_manager.h"
#include "gps_reader.h"
#include "../cabulator_settings.h"
#include <EEPROM.h>
#include <time.h>
#include <SPI.h>
#include <SD.h>

// Czytnik SD używa HSPI (Secondary SPI) aby nie kolidował z TFT (Primary SPI)
// TFT używa VSP (Primary SPI) - SD zostało przeniesione na osobny kanał

using namespace SDCARD;

// Instancja SPI dla karty SD (HSPI)
static SPIClass sdSPI(HSPI);

extern bool tripActive;
extern String currentTripPath;

// EEPROM adresy
#define TRIP_PATH_EEPROM_ADDR 25    // 41 bajtów (25-65) - max 40 znaków + null terminator
#define TRIP_PATH_EEPROM_MAX_LEN 40

namespace SDManager {

    // Zmienne globalne
    static bool sdReady = false;
    // currentTripPath jest globalna z main.cpp, nie potrzeba duplikatu tutaj!

    // Funkcja pomocnicza: zwraca aktualną datę i czas w formacie YYYY-MM-DD_HH-MM-SS
    static String getTimestamp() {
        // Najpierw spróbuj użyć czasu z GPS
        if (GPS::lastFix.dateTimeValid) {
            char buffer[20];
            snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d_%02d-%02d-%02d",
                GPS::lastFix.year, GPS::lastFix.month, GPS::lastFix.day,
                GPS::lastFix.hour, GPS::lastFix.minute, GPS::lastFix.second);
            return String(buffer);
        }
        
        // Fallback: użyj czasu systemowego
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", timeinfo);
        return String(buffer);
    }

    // Funkcja pomocnicza: zwraca aktualną datę w formacie YYYY-MM-DD
    static String getDateOnly() {
        // Najpierw spróbuj użyć daty z GPS
        if (GPS::lastFix.dateTimeValid) {
            char buffer[11];
            snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
                GPS::lastFix.year, GPS::lastFix.month, GPS::lastFix.day);
            return String(buffer);
        }
        
        // Fallback: użyj czasu systemowego
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
        return String(buffer);
    }

    bool init() {
        Serial.println("[SD] SD card initializing...");
        
        // Konfiguracja SPI dla karty SD - używamy HSPI
        sdSPI.begin(SDCARD::PIN_CLK, SDCARD::PIN_MISO_DATA, SDCARD::PIN_MOSI_DATA, SDCARD::PIN_CS_SD);
        
        // Inicjalizacja karty SD z instancją HSPI
        if (!SD.begin(SDCARD::PIN_CS_SD, sdSPI, 4000000)) {  // 4MHz speed for SD
            Serial.println("[SD] ERROR: Failed to initialize SD card!");
            sdReady = false;
            return false;
        }

        Serial.println("[SD] SD card initialized successfully!");
        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.printf("[SD] Card size: %lld MB\n", cardSize);
        sdReady = true;

        // Tworzenie głównych katalogów jeśli nie istnieją
        SD.mkdir("/logs");
        SD.mkdir("/logs/trips");

        return true;
    }

    bool isReady() {
        return sdReady;
    }

    String getLastTripPath() {
        char pathBuffer[TRIP_PATH_EEPROM_MAX_LEN + 1] = {0};
        for (int i = 0; i < TRIP_PATH_EEPROM_MAX_LEN; i++) {
            char c = EEPROM.read(TRIP_PATH_EEPROM_ADDR + i);
            if (c == 0) break;
            pathBuffer[i] = c;
        }
        String path(pathBuffer);
        if (path.length() > 0) {
            Serial.printf("[SD] Loaded trip path from EEPROM: %s\n", path.c_str());
        }
        return path;
    }

    void clearLastTripPath() {
        EEPROM.write(TRIP_PATH_EEPROM_ADDR, 0);
        EEPROM.commit();
        Serial.println("[SD] Trip path cleared from EEPROM");
    }

    String createTripSession() {
        if (!sdReady) {
            Serial.println("[SD] ERROR: SD card is not ready!");
            return "";
        }

        // Tworzenie ścieżki z datą i czasem
        String timestamp = getTimestamp();
        String tripPath = "/logs/trips/" + timestamp;
        
        // Debug: sprawdź źródło timestampu
        if (GPS::lastFix.dateTimeValid) {
            Serial.printf("[SD] Using GPS time: %04d-%02d-%02d %02d:%02d:%02d\n", 
                GPS::lastFix.year, GPS::lastFix.month, GPS::lastFix.day,
                GPS::lastFix.hour, GPS::lastFix.minute, GPS::lastFix.second);
        } else {
            Serial.println("[SD] WARNING: GPS time not valid, using system time!");
        }

        Serial.println("[SD] Creating trip session: " + tripPath);

        // Tworzenie folderu
        if (!SD.mkdir(tripPath)) {
            Serial.println("[SD] WARNING: Folder already exists or could not be created: " + tripPath);
        }

        currentTripPath = tripPath;

        // Zapis ścieżki do EEPROM
        for (int i = 0; i < (int)tripPath.length() && i < TRIP_PATH_EEPROM_MAX_LEN; i++) {
            EEPROM.write(TRIP_PATH_EEPROM_ADDR + i, tripPath[i]);
        }
        // Null terminator
        EEPROM.write(TRIP_PATH_EEPROM_ADDR + (int)min((int)tripPath.length(), TRIP_PATH_EEPROM_MAX_LEN), 0);
        EEPROM.commit();
        Serial.printf("[SD] Trip path saved to EEPROM: %s\n", tripPath.c_str());

        // Tworzenie nagłówka pliku gps_log.csv
        File gpsFile = SD.open(tripPath + "/gps_log.csv", FILE_WRITE);
        if (gpsFile) {
            gpsFile.println("Timestamp,Latitude,Longitude,Satellites,HDOP,Valid");
            gpsFile.close();
            Serial.println("[SD] File gps_log.csv created");
        }

        // Tworzenie nagłówka pliku obd_log.csv (dane tripu co 10 sekund)
        File obdFile = SD.open(tripPath + "/obd_log.csv", FILE_WRITE);
        if (obdFile) {
            obdFile.println("Timestamp,DistanceKm,FuelLiters,TotalCost");
            obdFile.close();
            Serial.println("[SD] File obd_log.csv created");
        }

        // Tworzenie nagłówka pliku trip_summary.csv (podsumowanie na koniec)
        File summaryFile = SD.open(tripPath + "/trip_summary.csv", FILE_WRITE);
        if (summaryFile) {
            summaryFile.println("Timestamp,DistanceKm,FuelLiters,TariffMode,TariffValue,TotalCost");
            summaryFile.close();
            Serial.println("[SD] File trip_summary.csv created");
        }

        return tripPath;
    }

    bool saveTripData(const String& tripPath, const TripData& data) {

        if (!sdReady) {

            Serial.println("[SD] ERROR: SD card is not ready!");
            return false;
        }

        File tripFile = SD.open(tripPath + "/trip_data.csv", FILE_APPEND);
        if (!tripFile) {

            Serial.println("[SD] ERROR: Failed to open trip_data.csv file!");
            return false;
        }

        // FORMAT DANYCH TRIPU
        // Format: Timestamp,DistanceKm,FuelLiters,TariffMode,TariffValue,TotalCost
        String line = String(millis()) + ",";
        line += String(data.distanceKm, 2) + ",";
        line += String(data.fuelUsedLiters, 3) + ",";
        line += String(data.tariffMode) + ",";
        line += String(data.tariffValue, 2) + ",";
        line += String(data.totalCost, 2);

        tripFile.println(line);
        tripFile.close();

        return true;
    }

    bool saveGPSData(const String& tripPath, const GPSData& data) {

        if (!sdReady) {

            Serial.println("[SD] ERROR: SD card is not ready!");
            return false;
        }

        File gpsFile = SD.open(tripPath + "/gps_log.csv", FILE_APPEND);
        if (!gpsFile) {

            Serial.println("[SD] ERROR: Failed to open gps_log.csv file!");
            return false;
        }

        // FORMAT DANYCH GPS    
        // Format: Timestamp,Latitude,Longitude,Satellites,HDOP,Valid
        String line = String(data.timestamp) + ",";
        line += String(data.latitude, 6) + ",";
        line += String(data.longitude, 6) + ",";
        line += String(data.satellites) + ",";
        line += String(data.hdop) + ",";
        line += (data.valid ? "1" : "0");

        gpsFile.println(line);
        gpsFile.close();

        return true;
    }

    void listTrips() {

        if (!sdReady) {

            Serial.println("[SD] ERROR: SD card is not ready!");
            return;
        }

        Serial.println("[SD] === AVAILABLE TRIPS ===");
        File root = SD.open("/logs/trips");

        if (!root || !root.isDirectory()) {

            Serial.println("[SD] Trips folder is missing or is a file!");
            return;
        }

        File file = root.openNextFile();
        int tripCount = 0;

        while (file) {

            if (file.isDirectory()) {

                Serial.println("  -> " + String(file.name()));
                tripCount++;
            }
            file = root.openNextFile();
        }

        Serial.println("[SD] Total trips: " + String(tripCount));
        root.close();
    }

    bool getLastTrip(String& tripPath) {

        if (!sdReady) {
            
            Serial.println("[SD] ERROR: SD card is not ready!");
            return false;
        }

        File root = SD.open("/logs/trips");
        if (!root || !root.isDirectory())
            return false;

        String lastTripName = "";
        File file = root.openNextFile();

        while (file) {

            if (file.isDirectory()) {

                String fileName = String(file.name());
                if (fileName > lastTripName)
                    lastTripName = fileName;
            }
            file = root.openNextFile();
        }
        root.close();

        if (lastTripName.isEmpty())
            return false;

        tripPath = "/logs/trips/" + lastTripName;
        return true;
    }

    void onGPSFix(const GPSData& data) {

        // Callback wywoływany przez GPS gdy pojawi się nowy fix
        // Sprawdzenie czy sesja tripu jest aktywna
        if (tripActive && isReady() && !currentTripPath.isEmpty())
            saveGPSData(currentTripPath, data);
    }

    void onTripUpdate(const TripUpdateData& data) {

        // Callback wywoływany przez OBD co ~10 sekund - zapisanie bieżących danych tripu
        // Sprawdzenie czy sesja tripu jest aktywna
        if (tripActive && isReady() && !currentTripPath.isEmpty()) {

            File obdFile = SD.open(currentTripPath + "/obd_log.csv", FILE_APPEND);
            if (!obdFile) {
                Serial.println("[SD] ERROR: Failed to open obd_log.csv file!");
                return;
            }

            // FORMAT: Timestamp,DistanceKm,FuelLiters,TotalCost
            String line = String(data.timestamp) + ",";
            line += String(data.distanceKm, 2) + ",";
            line += String(data.fuelUsedLiters, 3) + ",";
            line += String(data.totalCost, 2);

            obdFile.println(line);
            obdFile.close();

            Serial.printf("[SD] Trip update logged: dist=%.2f km, fuel=%.2f L, cost=%.2f\n",
                data.distanceKm, data.fuelUsedLiters, data.totalCost);
        }
    }

    void finalizeTrip(const TripData& data) {
        // Finalizacja sesji tripu - zapisanie podsumowania do trip_summary.csv
        if (!isReady() || currentTripPath.isEmpty()) {
            Serial.println("[SD] ERROR: Cannot finalize - SD path is missing!");
            return;
        }

        Serial.println("[SD] Finalizing trip session...");
        
        String summaryPath = currentTripPath + "/trip_summary.csv";
        
        // Sprawdź czy plik istnieje, jeśli nie - utwórz z nagłówkiem
        if (!SD.exists(summaryPath)) {
            Serial.println("[SD] trip_summary.csv doesn't exist, creating with header...");
            File summaryFile = SD.open(summaryPath, FILE_WRITE);
            if (summaryFile) {
                summaryFile.println("Timestamp,DistanceKm,FuelLiters,TariffMode,TariffValue,TotalCost");
                summaryFile.close();
            }
        }
        
        File summaryFile = SD.open(summaryPath, FILE_APPEND);
        if (!summaryFile) {
            Serial.println("[SD] ERROR: Failed to open trip_summary.csv file!");
            return;
        }

        // FORMAT: Timestamp,DistanceKm,FuelLiters,TariffMode,TariffValue,TotalCost
        String line = String(millis()) + ",";
        line += String(data.distanceKm, 2) + ",";
        line += String(data.fuelUsedLiters, 3) + ",";
        line += String(data.tariffMode) + ",";
        line += String(data.tariffValue, 2) + ",";
        line += String(data.totalCost, 2);

        summaryFile.println(line);
        summaryFile.close();

        Serial.println("[SD] Trip summary saved");
    }

}  // namespace SDManager
