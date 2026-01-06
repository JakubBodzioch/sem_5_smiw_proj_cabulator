#include "screen_tariff.h"
#include "background.h"
#include "gui_elements.h"
#include "screen_manager.h"
#include "screen_home.h"
#include <EEPROM.h>
#include <Arduino.h>

static TFT_eSPI* tftPtr = nullptr;
static Background* bgTariff = nullptr;

// Adresy w EEPROM
#define EEPROM_TARIFF_VALUE_ADDR 10
#define EEPROM_TARIFF_MODE_ADDR 14

// Globalne zmienne taryfy
float tariffValue = 3.00f;
TariffMode tariffMode = TARIFF_PER_KM;

void loadTariffFromEEPROM() {

    Serial.print("[TARIFF] tariff loaded from EEPROM: \n");
    EEPROM.get(EEPROM_TARIFF_VALUE_ADDR, tariffValue);
    Serial.print("[TARIFF] Loaded value: ");
    Serial.println(tariffValue, 4);

    if (isnan(tariffValue) || tariffValue < 0.01f || tariffValue > 100.0f) {

        Serial.println("[TARIFF] Invalid value, setting default 3.00");
        tariffValue = 3.00f; // default
    }

    uint8_t mode = EEPROM.read(EEPROM_TARIFF_MODE_ADDR);
    Serial.print("[TARIFF] Loaded mode: ");
    Serial.println(mode);
    if (mode > 1) mode = 0;
    tariffMode = (TariffMode)mode;
}

void saveTariffToEEPROM() {

    EEPROM.put(EEPROM_TARIFF_VALUE_ADDR, tariffValue);
    EEPROM.write(EEPROM_TARIFF_MODE_ADDR, (uint8_t)tariffMode);
    EEPROM.commit();
    Serial.print("[TARIFF] Tariff saved to EEPROM: \n");
    Serial.print("[TARIFF] Saved value: ");
    Serial.println(tariffValue, 4);
    Serial.print("[TARIFF] Saved mode: ");
    Serial.println((uint8_t)tariffMode);
}

void initTariffScreen(TFT_eSPI* tft) {

    tftPtr = tft;

    if (bgTariff) {

        delete bgTariff;
        bgTariff = nullptr;
    }

    bgTariff = new Background("/tariff.png");
    bgTariff->draw(*tft, *bgTariff->s_png, true);
    drawTariffValue(tft);
}

void drawTariffValue(TFT_eSPI* tft) {

    char buf[32];

    if (tariffMode == TARIFF_PER_KM) {

        sprintf(buf, "%.2f ZL/K", tariffValue);
        drawTextWithBackground(tft, buf, 300, 75, TR_DATUM, 2, TFT_YELLOW, TFT_BLACK, 120);

    } else {

        sprintf(buf, "%.2f ZL/L", tariffValue);
        drawTextWithBackground(tft, buf, 300, 75, TR_DATUM, 2, TFT_CYAN, TFT_BLACK, 120);
    }
}

void handleTariffTouch(uint16_t x, uint16_t y) {

    // -1zł
    if (x >= 10 && x < 150 && y >= 120 && y < 155) {

        tariffValue = max(0.0f, tariffValue - 1.0f);
        drawTariffValue(tftPtr);
        return;
    }
    // +1zł
    if (x >= 170 && x < 310 && y >= 120 && y < 155) {

        tariffValue += 1.0f;
        drawTariffValue(tftPtr);
        return;
    }
    // -0,5zł
    if (x >= 10 && x < 150 && y >= 160 && y < 175) {

        tariffValue = max(0.0f, tariffValue - 0.5f);
        drawTariffValue(tftPtr);
        return;
    }
    // -0,5zł
    if (x >= 170 && x < 310 && y >= 160 && y < 175) {

        tariffValue += 0.5f;
        drawTariffValue(tftPtr);
        return;
    }
    // Switch mode
    if (x >= 10 && x < 310 && y >= 195 && y < 230) {

        tariffMode = (tariffMode == TARIFF_PER_KM) ? TARIFF_PER_LITRE : TARIFF_PER_KM;
        drawTariffValue(tftPtr);
        return;
    }
    // Back
    if (x >= 260 && x < 310 && y >= 10 && y < 50) {
        
        saveTariffToEEPROM();
        initHomeScreen(tftPtr);
        currentScreen = SCREEN_HOME;
        return;
    }
}
