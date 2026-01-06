#include "gps_reader.h"
#include "sd_manager.h"
#include <TinyGPSPlus.h>
#include <sys/time.h>
#include <time.h>

#ifndef GPS_DEBUG_RAW
#define GPS_DEBUG_RAW 0
#endif

namespace GPS {

static TinyGPSPlus gps;                     // Obiekt TinyGPSPlus do parsowania NMEA
static HardwareSerial* port = &Serial2;     // Używamy Serial2 dla ESP32
static uint32_t lastSample = 0;             // Timestamp ostatniej próbki
Fix lastFix;                                // Ostatni fix GPS  

// Bufor na ostatnią linię NMEA
static char rawLogLines[5][128] = {{0}};    // 5 ostatnich linii NMEA
static uint8_t rawLinePos = 0;              // Pozycja w buforze
static char currentRawLine[128] = {0};      // Bieżąca linia NMEA
static uint8_t currentRawLinePos = 0;       // Pozycja w bieżącej linii

static inline uint32_t msSince(uint32_t t0) { 
    return millis() - t0; 
}

// Inicjalizacja GPS
void begin() {

    Serial.println("\n[GPS] Initializing GPS Module...");

    port->begin(BAUD_RATE, SERIAL_8N1, PIN_RX, PIN_TX);
    lastSample = millis();
    lastFix = Fix{};

    Serial.println("[GPS] ========================");
    Serial.printf("[GPS] Port: Serial2\n");
    Serial.printf("[GPS] Baud: %d\n", BAUD_RATE);
    Serial.printf("[GPS] RX Pin: %d\n", PIN_RX);
    Serial.printf("[GPS] TX Pin: %d\n", PIN_TX);
    Serial.println("[GPS] ========================");

    Serial.println("[GPS] GPS Module initialized");
}

// Polling GPS - zwraca true jeśli jest nowy fix
bool poll(Fix& out) {

    if (!port) return false;

    while (port->available() > 0) {

        char c = static_cast<char>(port->read());
        // Zebranie surowej linii NMEA dla debugu
        if (c == '\n' || c == '\r') {

            if (currentRawLinePos > 0) {

                // Zapisz linię do bufora ostatnich 5 linii
                currentRawLine[currentRawLinePos] = '\0';
                for (int i = 0; i < 4; ++i) {
                    strcpy(rawLogLines[i], rawLogLines[i+1]);
                }

                strncpy(rawLogLines[4], currentRawLine, sizeof(rawLogLines[4])-1);
                rawLogLines[4][sizeof(rawLogLines[4])-1] = '\0';
                currentRawLinePos = 0;
            }

        } else { // Normalny znak

            if (currentRawLinePos < sizeof(currentRawLine) - 2) 
                currentRawLine[currentRawLinePos++] = c;
        }
        gps.encode(c);
    }

    // Sprawdzenie czy minął czas próbkowania
    if (msSince(lastSample) >= SAMPLE_MS) {

        lastSample = millis();
        bool locValid = gps.location.isValid() && gps.location.age() < 2000;    // Valid jeśli <2s
        lastFix.valid = locValid;                                               // Ustaw valid
        lastFix.takenAtMs = lastSample;                                         // Timestamp próbki     
        lastFix.sats = gps.satellites.value();                                  // Liczba satelitów 
        uint32_t hd = gps.hdop.value();                                         // HDOP x100
        lastFix.hdop = (hd <= 0xFFFF) ? (uint16_t)hd : (uint16_t)65535;         // HDOP x100 z klipowaniem

        // Parsuj pozycję
        if (locValid) {

            lastFix.lat = gps.location.lat();   // Szerokość
            lastFix.lng = gps.location.lng();   // Długość
        }
        
        // Parsuj datę i czas z GPS
        if (gps.date.isValid() && gps.time.isValid()) {
            
            lastFix.year = gps.date.year();
            lastFix.month = gps.date.month();
            lastFix.day = gps.date.day();
            lastFix.hour = gps.time.hour();
            lastFix.minute = gps.time.minute();
            lastFix.second = gps.time.second();
            lastFix.dateTimeValid = true;
            
        } else 
            lastFix.dateTimeValid = false;
        
        // Przekaż dane wyjściowe
        out = lastFix;
        
        // Parsowanie danych na zapis do SD
        SDManager::GPSData gpsData;
        gpsData.latitude = lastFix.lat;
        gpsData.longitude = lastFix.lng;
        gpsData.satellites = lastFix.sats;
        gpsData.hdop = lastFix.hdop;
        gpsData.valid = lastFix.valid;
        gpsData.timestamp = lastFix.takenAtMs;
        SDManager::onGPSFix(gpsData);
        
        return true; 
    }
    return false; 
}

// Funkcja zwracająca wskaźnik do tablicy 5 ostatnich linii NMEA
const char* const* getLastRawLines() {

    static const char* ptrs[5];
    for (int i = 0; i < 5; ++i) ptrs[i] = rawLogLines[i];
    return ptrs;
}

// Synchronizacja czasu systemowego z GPS
bool setSystemTimeFromGPS(const Fix& fix) {

    if (!fix.dateTimeValid) {
        return false;
    }
    
    // Konwertowanie daty/czasu z GPS na structure timeval
    struct tm timeinfo = {};
    timeinfo.tm_year = fix.year - 1900;      // tm_year to lata od 1900
    timeinfo.tm_mon = fix.month - 1;         // tm_mon to 0-11
    timeinfo.tm_mday = fix.day;
    timeinfo.tm_hour = fix.hour;
    timeinfo.tm_min = fix.minute;
    timeinfo.tm_sec = fix.second;
    timeinfo.tm_isdst = -1;                  // Nieznane informacje o DST
    
    // Konwersja na timestamp
    time_t timestamp = mktime(&timeinfo);
    if (timestamp == -1) {

        Serial.println("[GPS] ERROR: Failed to convert GPS time to timestamp");
        return false;
    }
    
    // Ustawienie czasu systemowego
    struct timeval tv = {};
    tv.tv_sec = timestamp;
    tv.tv_usec = 0;
    
    if (settimeofday(&tv, nullptr) == 0) {

        Serial.printf("[GPS] System time synchronized: %04d-%02d-%02d %02d:%02d:%02d\n",
            fix.year, fix.month, fix.day, fix.hour, fix.minute, fix.second);
        return true;
        
    } else {

        Serial.println("[GPS] ERROR: Failed to set system time");
        return false;
    }
}

    // Sprawdzenie czy jest aktualny fix GPS
    bool hasLiveFix() {
        return gps.location.isValid() && gps.location.age() < 2000;
    }

    // Debugowanie statusu GPS
    void debugStatus() {

        Fix fix;
        if (poll(fix)) {
            
            if (fix.valid) {

                Serial.printf("[GPS] Lat: %.6f, Lng: %.6f, Sats: %d\n", 
                    fix.lat, fix.lng, fix.sats);
            }
        }
    }
}