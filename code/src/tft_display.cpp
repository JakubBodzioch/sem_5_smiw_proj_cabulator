#include "tft_display.h"

// =============================================================================
// KONFIGURACJA TFT - Ustawienia dla ekranu TFT
// =============================================================================

// Ustawienia pinu PWM dla podświetlenia
constexpr int BL_PIN = 17;    // Pin do sterowania podświetleniem
constexpr int BL_CH = 0;      // Kanał PWM

// Przechowywanie danych kalibracji ekranu
uint16_t calData[5] = { 243, 3566, 356, 3415, 1 }; // Przykładowe dane kalibracji

// =============================================================================
// FUNKCJE EKRANU TFT - Inicjalizacja i sterowanie podświetleniem
// =============================================================================

// Funkcja do ustawienia konfiguracji TFT (w tym kalibracja)
void initTFT(TFT_eSPI* tft) {
  tft->init();                  // Inicjalizacja wyświetlacza
  tft->setRotation(3);          // Ustawienie rotacji wyświetlacza
  tft->fillScreen(TFT_BLACK);   // Wypełnienie ekranu kolorem czarnym

  // Ustawienie danych kalibracji ekranu
  tft->setTouch(calData);       // Kalibracja ekranu (na podstawie wartości calData)

  // Inicjalizacja PWM dla podświetlenia
  ledcSetup(BL_CH, 5000, 8);    // Ustawienie PWM: częstotliwość 5kHz, rozdzielczość 8-bitowa
  ledcAttachPin(BL_PIN, BL_CH); // Przypisanie pinu do kanału PWM
}

// Funkcja do ustawienia jasności podświetlenia
void setBacklight(uint8_t brightness) {
  ledcWrite(BL_CH, brightness); // Ustawienie jasności (0-255)
}
