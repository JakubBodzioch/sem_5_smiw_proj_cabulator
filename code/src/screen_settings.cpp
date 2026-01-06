#include "screen_settings.h"
#include "screen_manager.h"
#include "screen_home.h"
#include "screen_brightness.h"
#include "screen_connection.h"
#include "background.h"
#include "gui_elements.h"
#include "screen_tariff.h"
#include <Arduino.h>

// =============================================================================
// GLOBALNE ZMIENNE - Przyciski na ekranie ustawień
// =============================================================================

static TFT_eSPI* tftPtr = nullptr;
static Background* bgSettings = nullptr;

// =============================================================================
// FUNKCJE EKRANU USTAWIEŃ - Inicjalizacja i obsługa dotyku
// =============================================================================

// Funkcja inicjalizująca ekran ustawień
void initSettingsScreen(TFT_eSPI* tft) {

  tftPtr = tft;
  if (bgSettings) {
      delete bgSettings;
      bgSettings = nullptr;
  }
 
  bgSettings = new Background("/settings.png");
  bgSettings->draw(*tft, *bgSettings->s_png, true);

  Serial.println("[SYSTEM] Settings screen initialized");
}

// Funkcja główna obsługująca dotyk na ekranie ustawień
void handleSettingsTouch(uint16_t x, uint16_t y) {

  // Powrót do ekranu głównego (górny prawy róg)
  if (x >= 260 && x < 310 && y >= 10 && y < 50) {

      initHomeScreen(tftPtr);
      currentScreen = SCREEN_HOME;
      return;
  }

  //przejście do ekranu taryfy (górny przycisk)
    if (x >= 10 && x < 310 && y >= 60 && y < 100) {

      initTariffScreen(tftPtr);
      currentScreen = SCREEN_TARIFF;
      return;
  }

  // Przejście do ekranu jasności (środkowy przycisk)
    if (x >= 10 && x < 310 && y >= 120 && y < 160) {

      initBrightnessScreen(tftPtr);
      currentScreen = SCREEN_BRIGHTNESS;
      return;
  }

  // Przejście do ekranu połączeń (dolny przycisk)
    if (x >= 10 && x < 310 && y >= 180 && y < 220) {
      
      initConnectionScreen(tftPtr);
      currentScreen = SCREEN_CONNECTION;
      return;
  }

}
