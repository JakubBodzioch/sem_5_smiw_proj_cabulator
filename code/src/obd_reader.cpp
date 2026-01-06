#include <BluetoothSerial.h>

#include "obd_reader.h"
#include "screen_trip.h"
#include "screen_tariff.h"
#include "sd_manager.h"
#include "../cabulator_settings.h"

// Zewnętrzne zmienne globalne z main.cpp i screen_tariff.cpp
extern String currentTripPath;

// =============================================================================
// MINIMALNA IMPLEMENTACJA OBD
// =============================================================================

namespace OBD {

BluetoothSerial SerialBT;

// Status połączenia OBD
bool btConnected = false;
bool elmReady = false;

// Wysyłanie komendy do OBD - zwraca odpowiedź w buforze
bool sendCmd(const char* cmd, char* response, int maxLen, int timeout = 1000) {

    while (SerialBT.available()) SerialBT.read();
    SerialBT.print(cmd);
    SerialBT.print("\r");
    
    int idx = 0;
    unsigned long start = millis();
    
    while (millis() - start < timeout && idx < maxLen - 1) {

        if (SerialBT.available()) {
            
            char c = SerialBT.read();
            if (c == '>') break;
            if (c != '\r' && c != '\n') {  // Ignoruj znaki nowej linii i powrotu 
                response[idx++] = c;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
    response[idx] = '\0';  // Eliminacja pustych znaków na końcu
    
    // Usunięcie trailing spaces (spacji na końcu odpowiedzi)
    while (idx > 0 && response[idx-1] == ' ') {
        response[--idx] = '\0';
    }
    
    return (idx > 0);
}

// Inicjalizacja połączenia OBD
bool init() {

#if OBD_SIMULATION_MODE
    Serial.println("\n[OBD] ===== SIMULATION MODE ENABLED =====");
    Serial.println("[OBD] Module status: SIMULATED");
    Serial.println("[OBD] Simulating: 100 km/h, 0.8 - 10 L/H");
    Serial.println("[OBD] ===================================\n");
    btConnected = true;
    elmReady = true;
    return true;

#else
    // Połączenie Bluetooth
    SerialBT.begin(OBD_CONFIG::DEVICE_NAME, true);
    uint8_t addr[6];
    sscanf(OBD_CONFIG::DEVICE_MAC_STR, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);
    
    bool connected = false;
    for (int i = 1; i <= 3; i++) {

        Serial.printf("[OBD] Module CONNECTING (TRY %d/3)\n", i);
        if (SerialBT.connect(addr)) {

            connected = true;
            break;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    if (!connected) {

        Serial.println("[OBD] Module NOT CONNECTED");
        btConnected = false;
        elmReady = false;
        return false;
    }
    
    // Inicjalizacja ELM327
    char resp[128];
    vTaskDelay(500 / portTICK_PERIOD_MS);
    sendCmd("ATZ", resp, sizeof(resp), 2000);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    sendCmd("ATE0", resp, sizeof(resp), 500);
    sendCmd("ATL0", resp, sizeof(resp), 500);
    sendCmd("ATS0", resp, sizeof(resp), 500);
    sendCmd("ATH0", resp, sizeof(resp), 500);
    sendCmd("ATSP6", resp, sizeof(resp), 1000);
    sendCmd("0100", resp, sizeof(resp), 3000);
    
    if (strstr(resp, "41") != NULL) {

        Serial.println("[OBD] Module CONNECTED BLUETOOTH + ELM");
        btConnected = true;
        elmReady = true;
        return true;

    } else {

        Serial.println("[OBD] Module CONNECTED ONLY BLUETOOTH");
        btConnected = true;
        elmReady = false;
        return false;
    }

#endif
}

// Odczyt odometru - zwraca KM
long readOdometer() {

#if OBD_SIMULATION_MODE
    // Symulacja opóźnienia (jak rzeczywisty ELM327)
    vTaskDelay(600 / portTICK_PERIOD_MS);
    // Symulacja: prędkość 50 km/h = 28 metrów co 2 sekundy
        static long simulatedOdoKiloMeters = 50000; // 50tys przebiegu startowego
        static unsigned long lastIncMs = 0;
        unsigned long now = millis();
        if (lastIncMs == 0) lastIncMs = now;
        if (now - lastIncMs >= 36000) {
            simulatedOdoKiloMeters += 1; // +1 km co 36 sekundy (100 km/h)
            lastIncMs += 36000;
        }
        return simulatedOdoKiloMeters;
#else
    char resp[64];
    if (!sendCmd(CarPID::ODOMETER, resp, sizeof(resp))) {
        return -1;
    }
        
    // Wyszukanie prefiksu odpowiedzi
    char* p = strstr(resp, CarPID::ODOMETER_RESP_PREFIX);
    if (p != NULL && strlen(p) >= 12) {

        // Wyciągnięcie 6 bajtów hex odpowiedzi
        char hexStr[7];
        strncpy(hexStr, p + 6, 6);
        hexStr[6] = '\0';
        
        long kilometers = strtol(hexStr, NULL, 16);
        
        if (kilometers >= 1000 && kilometers < 999999999) {

            Serial.printf("[ODO] Odometer read: %ld KM\n", kilometers);
            return kilometers;
        }
    }
    return -1; // Błąd odczytu

#endif
}

// Odczyt spalania przez MAF - zwraca L/h
float readFuelRate() {

#if OBD_SIMULATION_MODE
    // Symulacja opóźnienia (jak rzeczywisty ELM327)
    vTaskDelay(500 / portTICK_PERIOD_MS);
    // Symulacja spalania: losowo od 0.8 do 10 L/h
    float minL = 0.8f, maxL = 10.0f;
    float rand01 = (float)rand() / (float)RAND_MAX;
    float fuel = minL + (maxL - minL) * rand01;
    return fuel;

#else
    char resp[64];
    if (!sendCmd(CarPID::MAF, resp, sizeof(resp))) {
        return -1.0f;
    }
        
    // Szukanie prefiksu odpowiedzi
    char* p = strstr(resp, CarPID::MAF_RESP_PREFIX);
    if (p != NULL && strlen(p) >= 8) {
        // Wyciągnij 2 bajty hex
        char byteA[3], byteB[3];
        strncpy(byteA, p + 4, 2); byteA[2] = '\0';
        strncpy(byteB, p + 6, 2); byteB[2] = '\0';
        
        int a = strtol(byteA, NULL, 16);
        int b = strtol(byteB, NULL, 16);
        float maf = ((a * 256) + b) / 100.0f;
        
        float afr = 14.7f;
        float dens = 0.755f;
        float fuelRate = (maf / afr / dens) * 3.6f;
        Serial.printf("[FUEL] Fuel rate: %.2f L/H\n", fuelRate);
        return fuelRate;
    }
    return -1.0f; // Błąd odczytu

#endif
}

// Obliczenie kosztu przejazdu
float calculateCost() {

    if (tariffMode == TARIFF_PER_KM) 
        return distanceTraveled * tariffValue;
    else 
        return fuelUsed * tariffValue;
    
}

// Task OBD uruchomiony w tle (FreeRTOS)
void task(void* param) {

    static float lastOdo = -1.0f;
    static unsigned long lastMillis = 0;
    static unsigned long lastSDUpdate = 0;  // Timer dla zapisu na SD co 10 sekund
    
    while (true) {
        if (tripActive) {

            // Odczyt odometru
            long odoRaw = readOdometer();
            float dist = (odoRaw >= 0) ? (float)odoRaw : -1.0f;
            Serial.printf("[OBD_TASK] odoRaw=%ld, dist=%.2f\n", odoRaw, dist);
            
            // Odczyt paliwa
            float fuel = readFuelRate();
            Serial.printf("[OBD_TASK] fuel=%.2f\n", fuel);
            
            // LICZENIE tylko gdy nie zapauzowany
            if (!tripPaused) {

                unsigned long now = millis();
                
                // Inicjalizacja przy pierwszym odczycie
                if (lastMillis == 0 && dist >= 0 && fuel >= 0) {

                    lastMillis = now;
                    lastOdo = dist;
                    lastSDUpdate = now;
                }

                // Obliczanie przyrostów
                else if (lastMillis > 0 && dist >= 0) {
                    
                    float hours = (now - lastMillis) / 3600000.0f;
                    
                    // Dystans
                    float deltaDist = max(0.0f, dist - lastOdo);
                    if (deltaDist > 0.0001f) {
                        distanceTraveled += deltaDist;
                    }
                    lastOdo = dist;
                    
                    // Paliwo
                    if (fuel >= 0 && hours > 0) {
                        float fuelDelta = fuel * hours;
                        fuelUsed += fuelDelta;
                    }
                    
                    lastMillis = now;

                    // ========== ZAPIS NA SD CO 10 SEKUND ==========
                    if (now - lastSDUpdate >= 10000) {

                        if (SDManager::isReady() && !currentTripPath.isEmpty()) {
                            SDManager::TripUpdateData updateData;
                            updateData.distanceKm = distanceTraveled;
                            updateData.fuelUsedLiters = fuelUsed;
                            updateData.timestamp = now;

                            // Obliczenie całkowitego kosztu
                            int startedKm = (int)distanceTraveled + 1;
                            if (tariffMode == TARIFF_PER_KM)
                                updateData.totalCost = startedKm * tariffValue;
                            else
                                updateData.totalCost = fuelUsed * tariffValue;

                            SDManager::onTripUpdate(updateData);
                        }
                        lastSDUpdate = now;
                    }
                }

            } else {

                // Gdy zapauzowany - reset timera żeby po wznowieniu nie było skoku
                lastMillis = 0;
                lastSDUpdate = 0;
            }
            
        } else {

            // Reset przy nieaktywnym tripie
            lastOdo = -1.0f;
            lastMillis = 0;
            lastSDUpdate = 0;
        }
        
        vTaskDelay(2000 / portTICK_PERIOD_MS);  // Opóźnienie 2 sekundy
    }
}

} // namespace OBD