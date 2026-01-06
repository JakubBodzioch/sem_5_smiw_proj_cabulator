#include <Arduino.h>
#include <TFT_eSPI.h>
#include <EEPROM.h>
#include "esp_task_wdt.h"

#include "tft_display.h"
#include "gui_elements.h"
#include "gps_reader.h"
#include "obd_reader.h"
#include "background.h"
#include "screen_manager.h"
#include "sd_manager.h"

#include "screen_home.h"
#include "screen_settings.h"
#include "screen_brightness.h"
#include "screen_connection.h"
#include "screen_about.h"
#include "screen_gps.h"
#include "screen_gps-debug.h"
#include "screen_tariff.h"
#include "screen_trip.h"
#include "screen_obd.h"
#include "screen_obd-debug.h"

// =============================================================================
// GLOBALNE ZMIENNE
// =============================================================================

TFT_eSPI tft;
PNG png;
String currentTripPath = "";  // Ścieżka do folderu obecnej trasy


// =============================================================================
// TASKI RTOS - Wielowątkowe wykonanie
// =============================================================================

// Task OBD - czytanie danych z modułu OBD co 2 sekundy
void taskOBD(void* param) {
  OBD::task(param);
}

// Task GPS - odczyt i debug GPS co 10 sekund
void taskGPS(void* param) {
  static bool timeSync = false;     // Flaga synchronizacji czasu systemowego z GPS

  while (true) {

      GPS::Fix fix;

      if (GPS::poll(fix)) {

          GPS::debugStatus();
          // Synchronizacja czasu systemowego z GPS
          if (!timeSync && fix.dateTimeValid) {

              if (GPS::setSystemTimeFromGPS(fix)) {

                  timeSync = true;
                  Serial.println("[GPS] System time synchronized from GPS");
              }
          }

      }
      vTaskDelay(10000 / portTICK_PERIOD_MS); // Opóźnienie 10 sekund
  }
}

// =============================================================================
// SETUP - Inicjalizacja systemu
// =============================================================================

void setup() {

  Serial.begin(115200);
  
  // Wyłączenie WDT dla rdzenia 0 (uniknięcie resetów podczas długich operacji)
  disableCore0WDT();
  
  // Całkowite wyciszenie logów (przynajmniej próbowałem bo obd nadal pluje faktami)
  esp_log_level_set("*", ESP_LOG_NONE);
  
  Serial.println("\n\n\n[SYSTEM] ========== INITIALIZING ==========\n");

  // ========== FILESYSTEM ==========
  LittleFS.begin(true);
  Serial.println("[FS] ========================");
  Background::listFS("/");
  Serial.println("[FS] ========================\n");

  // ========== WYŚWIETLACZ ==========
  initTFT(&tft);
  Background bg("/loading.png");
  bg.draw(tft, png, true);
  currentScreen = SCREEN_WELCOME;

  // ========== EEPROM & JASNOŚĆ & TARYFA ==========
  EEPROM.begin(100);            // Zarezerowanie 100 bajtów w EEPROM
  initBrightnessModule();
  loadTariffFromEEPROM();

  // ========== INICJALIZACJA KARTY SD ==========
  if (!SDManager::init()) {
      Serial.println("[WARNING] SD Card initialization failed, continuing anyway\n");
  }
  
  // Rekalibracja dotyku po inicjalizacji SD
  uint16_t calData_recal[5] = { 243, 3566, 356, 3415, 1 };
  tft.setTouch(calData_recal);
  Serial.println("[TOUCH] Re-calibrated after SD init");

  // ========== INICJALIZACJA GPS I OBD ==========
  GPS::begin();

  if (!OBD::init()) {
      Serial.println("[WARNING] OBD initialization failed, continuing anyway\n");
  }

  Serial.println("[SYSTEM] ========== INIT COMPLETE ==========\n");

  // ========== INICJALIZACJA EKRANU GŁÓWNEGO ==========
  initHomeScreen(&tft);
  currentScreen = SCREEN_HOME;

  // ========== FREERTOS TASKS ==========
  xTaskCreate(taskOBD, "OBD", 8192, (void*)&tft, 2, NULL);
  xTaskCreate(taskGPS, "GPS", 4096, (void*)&tft, 1, NULL);
}


// =============================================================================
// LOOP - Główna pętla obsługi UI (blokujący dotyk i zmiana ekranów)
// =============================================================================

void loop() {
  uint16_t x, y;

  // ========== OBSŁUGA DOTYKU ==========
  static unsigned long lastTouchTime = 0;
  const unsigned long debounceDelay = 250;
  static bool touchReleased = true;

  if (tft.getTouch(&x, &y)) {
      x = tft.width() - x;
      y = tft.height() - y;

      if (currentScreen == SCREEN_BRIGHTNESS) {
          // Jasność ma specjalną obsługę bez debounce
          uint8_t newLevel = 0;
          handleBrightnessTouch(&tft, x, y, &newLevel);
      } else if (touchReleased) {
          touchReleased = false;
          unsigned long now = millis();
          
          if (now - lastTouchTime > debounceDelay) {
              lastTouchTime = now;
              
              // DELEGOWANIE OBSŁUGI DOTYKU DO AKTYWNEGO EKRANU
              switch (currentScreen) {
                  case SCREEN_HOME:
                      handleHomeTouch(x, y);
                      break;
                  case SCREEN_SETTINGS:
                      handleSettingsTouch(x, y);
                      break;
                  case SCREEN_TARIFF:
                      handleTariffTouch(x, y);
                      break;
                  case SCREEN_CONNECTION:
                      handleConnectionTouch(x, y);
                      break;
                  case SCREEN_ABOUT:
                      handleAboutTouch(x, y);
                      break;
                  case SCREEN_GPS:
                      handleGpsTouch(x, y);
                      break;
                  case SCREEN_GPS_DEBUG:
                      handleGpsDebugTouch(x, y);
                      break;
                  case SCREEN_TRIP:
                      handleTripTouch(x, y);
                      break;
                  case SCREEN_OBD:
                      handleObdTouch(x, y);
                      break;
                  case SCREEN_OBD_DEBUG:
                      handleObdDebugTouch(x, y);
                      break;
                  default:
                      break;
              }
          }
      }
  } else {
      touchReleased = true;
  }

  // ========== AKTUALIZACJA DANYCH W ZALEŻNOŚCI OD AKTYWNEGO EKRANU ==========
  static unsigned long lastScreenUpdate = 0;
  if (millis() - lastScreenUpdate > 1000) {

      if (currentScreen == SCREEN_HOME)
          updateGPSStatus(&tft);
      else if (currentScreen == SCREEN_TRIP) 
          updateTripStatus(&tft);
      else if (currentScreen == SCREEN_OBD_DEBUG)
          updateObdDebugScreen(&tft);
      else if (currentScreen == SCREEN_GPS_DEBUG)
          updateGpsDebugScreen(&tft);


      lastScreenUpdate = millis();
  }

  // ========== ZAPIS DANYCH TRASY DO EEPROM CO 30 SEKUND ==========
  static unsigned long lastTripSave = 0;
  if (tripActive && millis() - lastTripSave > 30000) {

      saveTripDataToEEPROM();
      lastTripSave = millis();
  }
}
