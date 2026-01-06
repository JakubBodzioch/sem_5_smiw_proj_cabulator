#include "screen_connection.h"
#include "screen_manager.h"
#include "background.h"
#include "screen_home.h"
#include "screen_gps.h"
#include "screen_about.h"
#include "screen_obd.h"
#include "screen_obd-debug.h"
#include <Arduino.h>

static TFT_eSPI* tftPtr = nullptr;
static Background* bgConnection = nullptr;

// Funkcja inicjalizująca ekran połączeń
void initConnectionScreen(TFT_eSPI* tft) {

    tftPtr = tft;
    if (bgConnection) {

        delete bgConnection;
        bgConnection = nullptr;
    }

    bgConnection = new Background("/connection.png");
    bgConnection->draw(*tft, *bgConnection->s_png, true);

    Serial.println("[SYSTEM] Connection screen initialized");
}

// Funkcja główna obsługująca dotyk na ekranie połączeń
void handleConnectionTouch(uint16_t x, uint16_t y) {

    // Przycisk 1: GPS INFO
    if (x >= 10 && x < 310 && y >= 60 && y < 100) {

        initGpsScreen(tftPtr);
        currentScreen = SCREEN_GPS;
        return;
    }

    // Przycisk 2: OBDI INFO
    if (x >= 10 && x < 310 && y >= 120 && y < 160) {

        initObdScreen(tftPtr);
        currentScreen = SCREEN_OBD;
        return;
    }

    // Przycisk 3: ABOUT
    if (x >= 10 && x < 310 && y >= 180 && y < 220) {

        initAboutScreen(tftPtr);
        currentScreen = SCREEN_ABOUT;
        return;
    }
    
    // Powrót do ekranu głównego (górny prawy róg)
    if (x >= 260 && x < 310 && y >= 10 && y < 50) {
        
        initHomeScreen(tftPtr);
        currentScreen = SCREEN_HOME;
    }
}
