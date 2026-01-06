#include "screen_gps-debug.h"
#include "gps_reader.h"
#include "screen_manager.h"
#include "screen_home.h"
#include "screen_gps.h"
#include "gui_elements.h"
#include <Arduino.h>

// Zmienne statyczne do przechowywania stanu ekranu
static TFT_eSPI* tftPtr = nullptr;
static char lastLatText[32] = "";
static char lastLonText[32] = "";
static char lastSatText[32] = "";

// Inicjalizacja ekranu diagnostyki GPS
void initGpsDebugScreen(TFT_eSPI* tft) {

    tftPtr = tft;
    tft->fillScreen(TFT_BLACK);
    // Tytuł
    drawText(tft, "GPS DIAGNOSTICS", 160, 10, TC_DATUM, 4, TFT_GREEN);
    // Opisy pól
    drawText(tft, "Latitude:", 10, 60, TL_DATUM, 2, TFT_GREEN);
    drawText(tft, "Longitude:", 10, 100, TL_DATUM, 2, TFT_GREEN);
    drawText(tft, "Satellites:", 10, 140, TL_DATUM, 2, TFT_GREEN);
    // Przycisk powrotu
    tft->fillRect(10, 200, 300, 40, TFT_DARKGREY);
    drawTextWithBackground(tft, "BACK", 160, 220, MC_DATUM, 2, TFT_WHITE, TFT_DARKGREY, 0);
    strcpy(lastLatText, "");
    strcpy(lastLonText, "");
    strcpy(lastSatText, "");
    Serial.println("[SYSTEM] GPS-DEBUG screen initialized");
}

// Aktualizacja ekranu diagnostyki GPS
void updateGpsDebugScreen(TFT_eSPI* tft) {

    if (!tft) return;

    // Aktualizacja danych GPS
    GPS::Fix fix;
    GPS::poll(fix);
    char latText[32];
    char lonText[32];
    char satText[32];

    if (fix.valid) {

        snprintf(latText, sizeof(latText), "%f", fix.lat);
        snprintf(lonText, sizeof(lonText), "%f", fix.lng);
        snprintf(satText, sizeof(satText), "%d", fix.sats);
    } else {

        strcpy(latText, "N/A");
        strcpy(lonText, "N/A");
        strcpy(satText, "N/A");
    }

    if (strcmp(latText, lastLatText) != 0) {
        drawTextWithBackground(tft, latText, 80, 60, TL_DATUM, 2, TFT_WHITE, TFT_BLACK, 200);
        strcpy(lastLatText, latText);
    }
    if (strcmp(lonText, lastLonText) != 0) {
        drawTextWithBackground(tft, lonText, 80, 100, TL_DATUM, 2, TFT_WHITE, TFT_BLACK, 200);
        strcpy(lastLonText, lonText);
    }
    if (strcmp(satText, lastSatText) != 0) {
        drawTextWithBackground(tft, satText, 80, 140, TL_DATUM, 2, TFT_WHITE, TFT_BLACK, 200);
        strcpy(lastSatText, satText);
    }
}

// Obsługa dotyku na ekranie diagnostyki GPS
void handleGpsDebugTouch(uint16_t x, uint16_t y) {

    if (x >= 10 && x <= 310 && y >= 200 && y <= 240) {
        
        initGpsScreen(tftPtr);
        currentScreen = SCREEN_GPS;
        return;
    }
}
