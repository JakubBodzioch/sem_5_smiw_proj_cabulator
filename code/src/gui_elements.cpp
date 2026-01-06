#include "gui_elements.h"

// =============================================================================
// Funkcje pomocnicze do rysowania tekstów
// =============================================================================

// Funkcja rysująca tekst bez tła
void drawText(TFT_eSPI* tft, const char* text, int x, int y, uint8_t datum, uint8_t font, uint16_t textColor)
{
  tft->setTextDatum(datum);
  tft->setTextFont(font);
  tft->setTextColor(textColor, TFT_BLACK);
  tft->drawString(text, x, y);
}

// Funkcja rysująca tekst z tłem
void drawTextWithBackground(
    TFT_eSPI* tft,
    const char* text,
    int x, int y,
    uint8_t datum,
    uint8_t font,
    uint16_t textColor,
    uint16_t bgColor,
    int16_t bgWidth
) {
    tft->setTextDatum(datum);
    tft->setTextFont(font);
    int16_t w = tft->textWidth(text);
    int16_t h = tft->fontHeight();
    int16_t vMargin = h / 5;
    int16_t h_bg = h + 2 * vMargin;
    int16_t w_bg = (bgWidth > 0) ? bgWidth : w;
    int bg_x = x, bg_y = y;
    switch (datum) {
      case TR_DATUM:
        bg_x = x - w_bg;
        bg_y = y - vMargin;
        break;
      case TL_DATUM:
        bg_x = x;
        bg_y = y - vMargin;
        break;
      case TC_DATUM:
      case MC_DATUM:
        bg_x = x - w_bg / 2;
        bg_y = y - vMargin;
        break;
      default:
        bg_x = x - w_bg / 2;
        bg_y = y - vMargin;
        break;
    }
    tft->fillRect(bg_x, bg_y, w_bg, h_bg, bgColor);
    tft->setTextColor(textColor);
    tft->drawString(text, x, y);
}
