/**
 * @file Screen_Manager.h
 * @brief Zarządzanie stanem ekranów aplikacji
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik odpowiada za globalny stan aktualnie wyświetlanego ekranu.
 * Dzięki temu w main.cpp wiemy, który handler dotyku wywołać.
 */

#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

/**
 * @enum ScreenState
 * @brief Wszystkie dostępne ekrany w systemie
 * 
 * Definiuje stany aplikacji używane do nawigacji między ekranami.
 */
enum ScreenState {
    SCREEN_WELCOME,     ///< Ekran powitalny (wyświetlany przy starcie)
    SCREEN_HOME,        ///< Ekran główny (domyślny widok)
    SCREEN_SETTINGS,    ///< Ekran ustawień głównych
    SCREEN_TARIFF,      ///< Ekran konfiguracji taryfy
    SCREEN_BRIGHTNESS,  ///< Ustawienia jasności i kalibracji dotyku
    SCREEN_CONNECTION,  ///< Ekran ustawień połączeń (GPS/OBD)
    SCREEN_GPS,         ///< Ekran informacyjny GPS
    SCREEN_OBD,         ///< Ekran informacyjny OBD
    SCREEN_OBD_DEBUG,   ///< Ekran diagnostyki OBD (szczegółowe dane)
    SCREEN_GPS_DEBUG,   ///< Ekran diagnostyki GPS (satelity, HDOP)
    SCREEN_ABOUT,       ///< Ekran informacji o autorze i systemie
    SCREEN_TRIP         ///< Ekran aktualnej trasy
};

/**
 * @brief Aktualny stan ekranu aplikacji
 * 
 * Zmienna globalna określająca który ekran jest obecnie wyświetlany.
 * Używana w głównej pętli do routowania zdarzeń dotyku.
 * 
 * @note Modyfikowana przez funkcje handleXxxTouch() przy zmianie ekranu
 */
extern ScreenState currentScreen;

#endif // SCREEN_MANAGER_H