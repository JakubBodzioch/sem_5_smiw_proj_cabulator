
#include "screen_gps.h"
#include "screen_manager.h"
#include "tft_display.h"
#include "background.h"
#include "screen_home.h"
#include "gui_elements.h"
#include "gps_reader.h"
#include "screen_gps-debug.h"
#include <Arduino.h>

static TFT_eSPI* tftPtr = nullptr;
static Background* bgGps = nullptr;

// Inicjalizacja ekranu GPS
void initGpsScreen(TFT_eSPI* tft) {

    tftPtr = tft;
    if (bgGps) {
        delete bgGps;
        bgGps = nullptr;
    }
    bgGps = new Background("/gps.png");
    bgGps->draw(*tft, *bgGps->s_png, true);
    Serial.println("[SYSTEM] GPS screen initialized");
}

// Obsługa dotyku na ekranie GPS
void handleGpsTouch(uint16_t x, uint16_t y) {

    // Debugowanie modułu GPS  
    if (x >= 10 && x < 310 && y >= 180 && y < 220) {

        initGpsDebugScreen(tftPtr);
        currentScreen = SCREEN_GPS_DEBUG;
        return;
    }

    // Powrót do ekranu głównego (górny prawy róg)
    if (x >= 260 && x < 310 && y >= 10 && y < 50) {

        initHomeScreen(tftPtr);
        currentScreen = SCREEN_HOME;
        return;
    }
}