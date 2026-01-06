#include "screen_home.h"
#include "gps_reader.h"
#include "obd_reader.h"
#include "screen_settings.h"
#include "screen_manager.h"
#include "screen_gps-debug.h"
#include "tft_display.h"
#include "background.h"
#include "gui_elements.h"
#include "screen_tariff.h"
#include "screen_trip.h"
#include <Arduino.h>

// =============================================================================
// GLOBALNE ZMIENNE - Deklaracje zmiennych globalnych używanych na ekranie głównym

// Bufor tekstu GPS i OBD statusu
static char lastGpsText[64] = "";
static char lastObdText[32] = "";

// =============================================================================

static TFT_eSPI* tftPtr = nullptr;
static Background* bgHome = nullptr;

// =============================================================================
// FUNKCJE EKRANU GŁÓWNEGO - Inicjalizacja, aktualizacja GPS i obsługa dotyku
// Timer do ograniczania odświeżania statusu GPS
// =============================================================================

// Funkcja inicjalizująca ekran główny
void initHomeScreen(TFT_eSPI* tft) {

    tftPtr = tft;
    if (bgHome) {
        
        delete bgHome;
        bgHome = nullptr;
    }

    if(tripActive)
        bgHome = new Background("/home_resume.png");
    else if(OBD::btConnected && OBD::elmReady)
        bgHome = new Background("/home_active.png");
    else
        bgHome = new Background("/home.png");

    // Rysowanie tła ekranu głównego
    bgHome->draw(*tft, *bgHome->s_png, true);

    // Reset bufora tekstu GPS statusu przy wejściu na ekran
    resetTextBuffer(lastGpsText, sizeof(lastGpsText));
    resetTextBuffer(lastObdText, sizeof(lastObdText));

    // Wyświetlanie taryfy w górnej części ekranu
    char tariffBuf[32];
    if (tariffMode == TARIFF_PER_KM) {

        sprintf(tariffBuf, "%.2f ZL/KM", tariffValue);
        drawTextWithBackground(tft, tariffBuf, 300, 70, TR_DATUM, 2, TFT_YELLOW, TFT_BLACK, 120);

    } else {

        sprintf(tariffBuf, "%.2f ZL/L", tariffValue);
        drawTextWithBackground(tft, tariffBuf, 300, 70, TR_DATUM, 2, TFT_CYAN, TFT_BLACK, 120);
    }

    Serial.println("[SYSTEM] Home screen initialized");
}

// Wyświetlanie statusu GPS i OBD na ekranie głównym
void updateGPSStatus(TFT_eSPI* tft) {
  
    if (!tft) return;
    GPS::poll(GPS::lastFix); // Pobierz najnowszą próbkę GPS

    // Wyświetlanie uproszczonego statusu GPS
    char gpsBuf[16];
    uint16_t gpsColor = TFT_RED;
    if (!GPS::lastFix.valid) {

        strcpy(gpsBuf, "NO FIX");
        gpsColor = TFT_RED;

    } else if (GPS::lastFix.sats < 5) {

        strcpy(gpsBuf, "WEAK");
        gpsColor = TFT_ORANGE;

    } else {

        strcpy(gpsBuf, "STRONG");
        gpsColor = TFT_GREEN;
    }

    // Wyświetlanie statusu OBD
    char obdBuf[32];
    bool forceRedraw = false;
    
    if (OBD::btConnected && OBD::elmReady)
        strcpy(obdBuf, "CONNECTED");
    else if (OBD::btConnected) 
        strcpy(obdBuf, "BT ONLY");
    else
        strcpy(obdBuf, "NO LINK");

    // Mechanizm podmiany tła przy zmianie statusu OBD
    static bool lastObdWasConnected = false;
    bool nowConnected = (OBD::btConnected && OBD::elmReady);
    static String lastBgPath = "";
    const char* desiredBg = nowConnected ? "/home_active.png" : "/home.png";

    if (lastBgPath != desiredBg) {

        if (bgHome) { delete bgHome; bgHome = nullptr; }
        bgHome = new Background(desiredBg);
        bgHome->draw(*tft, *bgHome->s_png, true);
        lastBgPath = desiredBg;
        forceRedraw = true;
    }
    lastObdWasConnected = nowConnected;

    // Rysowanie napisów tylko jeśli się zmieniły lub wymuszone GPS
    if (strcmp(gpsBuf, lastGpsText) != 0 || forceRedraw) {

        drawTextWithBackground(tft, gpsBuf, 300, 110, TR_DATUM, 2, gpsColor, TFT_BLACK, 140);
        strcpy(lastGpsText, gpsBuf);
    }

    // Rysowanie napisów tylko jeśli się zmieniły lub wymuszone OBD
    if (strcmp(obdBuf, lastObdText) != 0 || forceRedraw) {

        drawTextWithBackground(tft, obdBuf, 300, 130, TR_DATUM, 2, OBD::btConnected ? TFT_GREEN : TFT_RED, TFT_BLACK, 140);
        strcpy(lastObdText, obdBuf);
    }
}

// Obsługa dotyku na ekranie głównym
void handleHomeTouch(uint16_t x, uint16_t y) {

  // Przejście do ustawień (górny prawy róg)
  if (x >= 260 && x < 310 && y >= 10 && y < 50) {

    initSettingsScreen(tftPtr);
    currentScreen = SCREEN_SETTINGS;
    return;
  }

  // Rozpoczęcie jazdy (dolny prostokąt) tylko jeśli OBD połączone
  if (x >= 20 && x < 220 && y >= 200 && y < 240) {

    if (OBD::btConnected && OBD::elmReady) {

        initTripScreen(tftPtr);
        currentScreen = SCREEN_TRIP;

    } else {
        Serial.println("[ERROR] Cannot start trip if OBD is not connected");
    }
    return;
  }
}
