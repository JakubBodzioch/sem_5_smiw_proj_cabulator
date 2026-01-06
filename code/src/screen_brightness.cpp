#include "screen_brightness.h"
#include "gui_elements.h"
#include "tft_display.h"
#include "screen_settings.h"
#include "screen_manager.h"
#include "screen_home.h"
#include "background.h"
#include <Arduino.h>
#include <EEPROM.h>

#define BRIGHTNESS_ADDR 0 // Adres w EEPROM do przechowywania poziomu jasności

static TFT_eSPI* tftPtr = nullptr;
static Background* bgBrightness = nullptr;
static char lastText[8] = "";

uint8_t brightnessLevel = 250;

// Funkcja ustawiająca jasność podświetlenia ekranu
void initBrightnessModule() {

    uint8_t saved = EEPROM.read(BRIGHTNESS_ADDR);
    brightnessLevel = saved;

    if (brightnessLevel < 5) brightnessLevel = 255;

    setBacklight(brightnessLevel);
    Serial.print("[BRIGHTNESS] Brightness module initialized with level: ");
    Serial.println(brightnessLevel);
}

// Funkcja rysująca procentową jasność na ekranie
void drawBrightnessPercent(TFT_eSPI* tft, uint8_t brightness) {

    char buf[8];
    uint8_t percent = (uint8_t)((brightness * 100) / 255);
    sprintf(buf, "%d%%", percent);

    if (strcmp(buf, lastText) != 0) {

        drawTextWithBackground(tft, buf, 300, 75, TR_DATUM, 2, TFT_WHITE, TFT_BLACK, 80);
        strcpy(lastText, buf);
    }
}

// Funkcja inicjalizująca ekran jasności
void initBrightnessScreen(TFT_eSPI* tft) {

    tftPtr = tft;
    if (bgBrightness) {

        delete bgBrightness;
        bgBrightness = nullptr;
    }
    
    bgBrightness = new Background("/brightness.png");
    bgBrightness->draw(*tft, *bgBrightness->s_png, true);
    setBacklight(brightnessLevel);

    // Reset bufora tekstu jasności przy wejściu na ekran
    resetTextBuffer(lastText, sizeof(lastText));
    // Wyświetlanie aktualnej jasności w procentach
    drawBrightnessPercent(tftPtr, brightnessLevel);

    Serial.println("[SYSTEM] Brightness screen initialized");
}

// Funkcja główna obsługująca dotyk na ekranie jasności
void handleBrightnessTouch(TFT_eSPI* tft, uint16_t x, uint16_t y, uint8_t* levelOut) {

    // Przycisk -10
    if (x >= 10 && x < 150 && y >= 130 && y < 170) {

        brightnessLevel = constrain(brightnessLevel - 10, 10, 255);
        setBacklight(brightnessLevel);
        drawBrightnessPercent(tftPtr, brightnessLevel);
        return;
    }

    // Przycisk +10
    if (x >= 170 && x < 310 && y >= 130 && y < 170) {

        brightnessLevel = constrain(brightnessLevel + 10, 10, 255);
        setBacklight(brightnessLevel);
        drawBrightnessPercent(tftPtr, brightnessLevel);
        return;
    }
    
    // Powrót do ekranu głównego (górny prawy róg)
    if (x >= 260 && x < 310 && y >= 10 && y < 50) {

        EEPROM.write(BRIGHTNESS_ADDR, brightnessLevel);
        EEPROM.commit();
        Serial.print("[BRIGHTNESS] Brightness saved to EEPROM: ");
        Serial.println(brightnessLevel);
        initHomeScreen(tftPtr);
        currentScreen = SCREEN_HOME;
        return;
    }
}