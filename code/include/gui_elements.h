/**
 * @file GUI_Elements.h
 * @brief Komponenty interfejsu użytkownika - elementy GUI dla TFT
 * @version 1.0
 * @date 2025-01-20
 * 
 * Plik zawiera struktury i funkcje do tworzenia elementów GUI.
 * Definiuje style przycisków, tekstu oraz uniwersalne funkcje rysowania
 * używane we wszystkich ekranach aplikacji.
 */

#ifndef GUI_ELEMENTS_H
#define GUI_ELEMENTS_H

#include <TFT_eSPI.h>

/**
 * @brief Rysuje tekst na ekranie TFT bez tła
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 * @param text Tekst do wyświetlenia (C-string)
 * @param x Współrzędna X pozycji tekstu
 * @param y Współrzędna Y pozycji tekstu
 * @param datum Punkt wyrównania tekstu (TL_DATUM, MC_DATUM, TR_DATUM, etc.)
 * @param font Numer czcionki TFT_eSPI (1-8)
 * @param textColor Kolor tekstu w formacie RGB565
 * 
 * @note Funkcja bez tła - tekst rysowany bezpośrednio na aktualnym tle ekranu
 */
void drawText(TFT_eSPI* tft, const char* text, int x, int y, uint8_t datum, uint8_t font, uint16_t textColor);

/**
 * @brief Rysuje tekst z kolorowym tłem
 * 
 * Renderuje prostokątne tło pod tekstem, przydatne do tworzenia
 * etykiet, przycisków lub wyróżnionych napisów.
 * 
 * @param tft Wskaźnik do obiektu wyświetlacza TFT
 * @param text Tekst do wyświetlenia
 * @param x Współrzędna X pozycji tekstu
 * @param y Współrzędna Y pozycji tekstu
 * @param datum Punkt odniesienia tekstu (np. MC_DATUM dla środka)
 * @param font Numer czcionki TFT_eSPI
 * @param textColor Kolor tekstu (RGB565)
 * @param bgColor Kolor tła (RGB565)
 * @param bgWidth Całkowita szerokość prostokąta tła [px] (domyślnie 0 = auto)
 * 
 * @warning Tło jest rysowane przed tekstem, może nadpisać istniejącą grafikę
 * @note bgWidth=0 oznacza automatyczną szerokość (dopasowaną do tekstu)
 */
void drawTextWithBackground(
    TFT_eSPI* tft,
    const char* text,
    int x, int y,
    uint8_t datum,
    uint8_t font,
    uint16_t textColor,
    uint16_t bgColor,
    int16_t bgWidth = 0
);

/**
 * @brief Resetuje bufor tekstowy wypełniając go zerami
 * 
 * Funkcja pomocnicza do czyszczenia buforów char[] przed
 * formatowaniem nowego tekstu.
 * 
 * @param buf Wskaźnik do bufora do wyczyszczenia
 * @param size Rozmiar bufora w bajtach
 * 
 * @note Funkcja inline dla wydajności
 */
inline void resetTextBuffer(char* buf, size_t size) {
    memset(buf, 0, size);
}

#endif // GUI_ELEMENTS_H