
#include "screen_obd.h"
#include "screen_obd-debug.h"
#include "screen_manager.h"
#include "tft_display.h"
#include "background.h"
#include "screen_home.h"
#include "gui_elements.h"
#include "gps_reader.h"
#include <Arduino.h>

static TFT_eSPI* tftPtr = nullptr;
static Background* bgGps = nullptr;

void initObdScreen(TFT_eSPI* tft) {

    tftPtr = tft;

    if (bgGps) {

        delete bgGps;
        bgGps = nullptr;
    }

    bgGps = new Background("/obd.png");
    bgGps->draw(*tft, *bgGps->s_png, true);
    Serial.println("[SYSTEM] OBD screen initialized");
}

void handleObdTouch(uint16_t x, uint16_t y) {

    // Włączenie ekranu debugowania OBD
    if (x >= 10 && x < 310 && y >= 180 && y < 220) {

        initObdDebugScreen(tftPtr);
        currentScreen = SCREEN_OBD_DEBUG;
        return;
    }

    // Powrót do ekranu głównego
    if (x >= 260 && x < 310 && y >= 10 && y < 50) {
        
        initHomeScreen(tftPtr);
        currentScreen = SCREEN_HOME;
        return;
    }
}

