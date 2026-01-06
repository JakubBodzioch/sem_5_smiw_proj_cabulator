#include "screen_obd-debug.h"
#include "screen_obd.h"
#include "screen_manager.h"
#include "obd_reader.h"
#include "screen_home.h"
#include "gui_elements.h"
#include <Arduino.h>

static TFT_eSPI* tftPtr = nullptr;
static char lastOdoText[32] = "";
static char lastFuelText[32] = "";

void initObdDebugScreen(TFT_eSPI* tft) {

    tftPtr = tft;
    tft->fillScreen(TFT_BLACK);
    
    // Tytuł
    drawText(tft, "OBD DIAGNOSTICS", 160, 10, TC_DATUM, 4, TFT_SKYBLUE);
    
    // Opisy pól
    drawText(tft, "Odometer:", 10, 60, TL_DATUM, 2, TFT_SKYBLUE);
    drawText(tft, "Fuel Rate:", 10, 120, TL_DATUM, 2, TFT_SKYBLUE);
    
    // Przycisk powrotu
    tft->fillRect(10, 200, 300, 40, TFT_DARKGREY);
    drawTextWithBackground(tft, "BACK", 160, 220, MC_DATUM, 2, TFT_WHITE, TFT_DARKGREY, 0);
    
    strcpy(lastOdoText, "");
    strcpy(lastFuelText, "");
    
    Serial.println("[SYSTEM] OBD-DEBUG screen initialized");
}

void updateObdDebugScreen(TFT_eSPI* tft) {

    if (!tft) return;
    
    // Odczyt danych
    long odo = OBD::readOdometer();
    float fuel = OBD::readFuelRate();
    
    char odoText[32];
    char fuelText[32];
    
    if (odo > 0)
        sprintf(odoText, "%ld KM", odo);
    else
        sprintf(odoText, "N/A");
    
    if (fuel >= 0)
        sprintf(fuelText, "%.2f L/H", fuel);
    else
        sprintf(fuelText, "N/A");
    
    // Update tylko przy zmianie
    if (strcmp(odoText, lastOdoText) != 0) {
        drawTextWithBackground(tft, odoText, 10, 80, TL_DATUM, 4, TFT_WHITE, TFT_BLACK, 300);
        strcpy(lastOdoText, odoText);
    }
    
    if (strcmp(fuelText, lastFuelText) != 0) {
        drawTextWithBackground(tft, fuelText, 10, 140, TL_DATUM, 4, TFT_WHITE, TFT_BLACK, 300);
        strcpy(lastFuelText, fuelText);
    }
}

void handleObdDebugTouch(uint16_t x, uint16_t y) {

    // Przycisk powrotu
    if (x >= 10 && x < 310 && y >= 200 && y < 240) {

        initObdScreen(tftPtr);
        currentScreen = SCREEN_OBD;
        return;
    }
}
