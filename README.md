# 🚕 Cabulator | ESP32-Based Taxi Telematics System

**Cabulator** to zaawansowany system telematyczny zaprojektowany dla pojazdów prywatnych, pełniący funkcję inteligentnego taksometru. Urządzenie integruje dane pozycjonowania satelitarnego z diagnostyką pokładową pojazdu (OBD-II), oferując pełną kontrolę nad kosztami i parametrami przejazdu w czasie rzeczywistym.

---

## 🛠️ Specyfikacja Sprzętowa

Sercem urządzenia jest **ESP32-WROOM-32 (Dev-kit v1)**, wybrany ze względu na wysoką wydajność (240 MHz) oraz natywne wsparcie dla wielowątkowości (FreeRTOS).

### Kluczowe Komponenty:
* **Jednostka Centralna (ESP32):** Obsługa dwóch niezależnych magistrali SPI (**VSPI** dla wyświetlacza, **HSPI** dla karty SD), co eliminuje konflikty i opóźnienia w zapisie danych. Wspiera profil **Bluetooth SPP** niezbędny do komunikacji z interfejsami OBD-II.
* **Lokalizacja (GY-NEO6M V2):** Moduł GPS komunikujący się przez UART. Zapewnia dokładność do 2,5 m oraz synchronizację czasu systemowego z wzorcem satelitarnym.
* **Interfejs (TFT 2.8" ST7789):** Wyświetlacz o rozdzielczości 240x320 z panelem dotykowym. Zoptymalizowany pod kątem pracy w słońcu (montaż na desce rozdzielczej) i obsługiwany przez bibliotekę **TFT_eSPI**.
* **Diagnostyka (Vgate iCar2):** Interfejs Bluetooth ELM327 v2.2, charakteryzujący się błyskawicznym czasem łączenia z ECU i szeroką kompatybilnością (pojazdy od 1996 r.).

---

## 🏗️ Architektura Systemu

Urządzenie pracuje w oparciu o model wielozadaniowy **RTOS**, co gwarantuje płynność interfejsu nawet podczas intensywnego przetwarzania danych w tle.



* **Task GPS:** Niezależne parsowanie ramek NMEA i aktualizacja współrzędnych.
* **Task OBD:** Odczyt parametrów silnika, zużycia paliwa i przebiegu przez Bluetooth.
* **Main Loop:** Obsługa logiki UI, 12 ekranów funkcyjnych oraz interakcji dotykowych.

---

## 📱 Funkcjonalności Interfejsu

System oferuje **12 dedykowanych ekranów**, które pozwalają na pełną personalizację i monitoring:
* **Ekran Aktywnej Trasy:** Podgląd dystansu i kosztu na żywo.
* **Ustawienia Taryf:** Możliwość wyboru rozliczenia za kilometr lub litr paliwa (dane zapisywane trwale w pamięci NVS).
* **Diagnostyka:** Szczegółowy wgląd w parametry satelitów (SNR, pozycja) oraz dane z komputera pokładowego.
* **Personalizacja:** Regulacja jasności ekranu i konfiguracja połączeń bezprzewodowych.

---

## 💾 Rejestracja Danych (Blackbox)

Wszystkie dane przejazdu są archiwizowane na karcie SD w zorganizowanej strukturze katalogów czasowych:
* **Logi (.csv):** Szczegółowy zapis parametrów trasy sekunda po sekundzie.
* **Statystyki:** Podsumowanie końcowe (całkowity koszt, spalanie, dystans) generowane po zakończeniu kursu.

---

## 🔧 Technologie
* **Język:** C++ (Arduino/ESP-IDF)
* **System Operacyjny:** FreeRTOS
* **Biblioteki:** TFT_eSPI, TinyGPSPlus, BluetoothSerial, SPI, FS, SD
* **Komunikacja:** UART, SPI (Dual Bus), Bluetooth SPP
