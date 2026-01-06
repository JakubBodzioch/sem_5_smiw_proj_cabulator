#include "screen_about.h"
#include "screen_home.h"
#include "screen_manager.h"
#include "tft_display.h"
#include "background.h"
#include <Arduino.h>

static TFT_eSPI* tftPtr = nullptr;
static Background* bgAbout = nullptr;

// Inicjalizacja ekranu "About"
void initAboutScreen(TFT_eSPI* tft) {

    tftPtr = tft;
    
    if (bgAbout) {

        delete bgAbout;
        bgAbout = nullptr;
    }

    bgAbout = new Background("/about.png");
    bgAbout->draw(*tft, *bgAbout->s_png, true);
    Serial.println("[SYSTEM] About screen initialized");
}

// Obsługa dotyku na ekranie "About"
void handleAboutTouch(uint16_t x, uint16_t y) {

    // Przycisk powrotu (lewy górny róg)
    if (x >= 260 && x < 310 && y >= 10 && y < 50) {
        
        initHomeScreen(tftPtr);
        currentScreen = SCREEN_HOME;
        return;
    }
}
