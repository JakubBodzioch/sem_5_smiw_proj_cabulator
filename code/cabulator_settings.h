#ifndef CABULATOR_SETTINGS_H
#define CABULATOR_SETTINGS_H

// =============================================================================
// CABULATOR SETTINGS
// =============================================================================
// Centralny plik konfiguracyjny dla całego projektu
// =============================================================================


// =============================================================================
// OBD READER KONFIGURACJA
// =============================================================================

// Preprocessor define dla symulacji OBD
#define OBD_SIMULATION_MODE 0                                       // 1 = symulacja, 0 = rzeczywiste OBD

namespace OBD_CONFIG {
    constexpr const char* DEVICE_NAME = "V-LINK";                   // Nazwa modułu Bluetooth OBD-II
    constexpr const char* DEVICE_MAC_STR = "10:21:3e:4e:e5:84";     // Adres MAC modułu Bluetooth OBD-II
}

// PID definicje dla Volvo V40 2014+
namespace CarPID {
    constexpr const char* ODOMETER = "22DD01";                      // Odometer PID 
    constexpr const char* ODOMETER_RESP_PREFIX = "62DD01";          // Odpowiedź odometru
    constexpr const char* MAF = "0110";                             // Mass Air Flow (backup dla paliwa)
    constexpr const char* MAF_RESP_PREFIX = "4110";                 // Odpowiedź MAF
}


// =============================================================================
// GPS READER KONFIGURACJA
// =============================================================================
namespace GPS {
    constexpr int PIN_RX = 26;          // Pin RX dla GPS
    constexpr int PIN_TX = 25;          // Pin TX dla GPS
    constexpr int BAUD_RATE = 9600;     // Prędkość UART
    constexpr int SAMPLE_MS = 1000;     // Częstotliwość próbkowania
}  // namespace GPS


// =============================================================================
// SD CARD KONFIGURACJA (SPI)
// =============================================================================
namespace SDCARD {
    constexpr int PIN_CLK = 27;             // SCK (Serial Clock)
    constexpr int PIN_MOSI_DATA = 14;       // MOSI (Master Out, Slave In)
    constexpr int PIN_MISO_DATA = 12;       // MISO (Master In, Slave Out)
    constexpr int PIN_CS_SD = 13;           // CS (Chip Select)
}  // namespace SDCARD

#endif
