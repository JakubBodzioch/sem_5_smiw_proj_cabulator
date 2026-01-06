/**
 * @file Background.h
 * @brief Obsługa tła aplikacji - rysowanie PNG na ekranie TFT
 * @version 1.0
 * @date 2025-01-20
 *
 * Plik zawiera klasę i funkcje do obsługi tła ekranu.
 * Pozwala na rysowanie tła z pliku PNG, zarządzanie ścieżką pliku
 * oraz obsługę systemu plików LittleFS.
 */

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <Arduino.h>
#include <FS.h>
#include <PNGdec.h>
#include <LittleFS.h>
#include <TFT_eSPI.h>

using namespace fs;

// Forward declaration
class TFT_eSPI;

/**
 * @class Background
 * @brief Klasa do zarządzania tłem ekranu TFT
 *
 * Umożliwia ładowanie i wyświetlanie obrazów PNG jako tła aplikacji.
 * Obsługuje system plików LittleFS oraz dekodowanie PNG w locie.
 *
 * @note Wymaga zainicjalizowanego LittleFS przed użyciem
 */
class Background {

public:

    /// @name Statyczne zmienne konfiguracyjne
    /// @{
    static int s_offX;              ///< Offset poziomy przy rysowaniu PNG
    static int s_offY;              ///< Offset pionowy przy rysowaniu PNG
    static uint16_t s_lineBuf[320]; ///< Bufor linii do rysowania PNG (szerokość ekranu)
    static File s_pngFile;          ///< Uchwyt do otwartego pliku PNG
    static TFT_eSPI *s_tft;         ///< Wskaźnik do obiektu wyświetlacza TFT
    static PNG *s_png;              ///< Wskaźnik do dekodera PNG
    /// @}

    /**
     * @brief Konstruktor klasy Background
     * @param path Ścieżka do pliku PNG w systemie plików LittleFS
     */
    explicit Background(const String &path);

    /**
     * @brief Ustawia nową ścieżkę do pliku PNG
     * @param path Nowa ścieżka do pliku PNG
     */
    void setPath(const String &path);

    /**
     * @brief Zwraca aktualną ścieżkę do pliku PNG
     * @return Ścieżka do pliku PNG jako String
     */
    String path() const;

    /**
     * @brief Rysuje tło na ekranie TFT
     * @param tft Referencja do obiektu wyświetlacza TFT
     * @param png Referencja do obiektu dekodera PNG
     * @param center Czy wyśrodkować obraz na ekranie (domyślnie true)
     * @return true jeśli rysowanie się powiodło, false w przypadku błędu
     */
    bool draw(TFT_eSPI &tft, PNG &png, bool center = true);

    /**
     * @brief Listuje pliki w systemie plików LittleFS
     * @param dir Katalog do listowania (domyślnie "/")
     *
     * Wypisuje listę plików na Serial w celach debugowania.
     */
    static void listFS(const char *dir = "/");

    /// @name Callbacki dla dekodera PNG
    /// @{

    /**
     * @brief Callback otwierający plik PNG
     * @param filename Nazwa pliku do otwarcia
     * @param pFileSize[out] Wskaźnik do zmiennej przechowującej rozmiar pliku
     * @return Uchwyt do pliku lub nullptr w przypadku błędu
     */
    static void *pngOpen(const char *filename, int32_t *pFileSize);

    /**
     * @brief Callback zamykający plik PNG
     * @param handle Uchwyt do pliku zwrócony przez pngOpen()
     */
    static void pngClose(void *handle);

    /**
     * @brief Callback czytający dane z pliku PNG
     * @param pFile Wskaźnik do struktury PNGFILE
     * @param pBuff Bufor docelowy na dane
     * @param iLen Liczba bajtów do odczytania
     * @return Liczba faktycznie odczytanych bajtów
     */
    static int32_t pngRead(PNGFILE *pFile, uint8_t *pBuff, int32_t iLen);

    /**
     * @brief Callback ustawiający pozycję w pliku PNG
     * @param pFile Wskaźnik do struktury PNGFILE
     * @param iPosition Nowa pozycja w pliku
     * @return Nowa pozycja lub kod błędu
     */
    static int32_t pngSeek(PNGFILE *pFile, int32_t iPosition);

    /// @}

    /**
     * @brief Callback wywoływany podczas dekodowania każdej linii PNG
     * @param p Wskaźnik do struktury PNGDRAW z danymi linii
     * @return 1 jeśli sukces, 0 w przypadku błędu
     *
     * Funkcja renderuje zdekodowaną linię bezpośrednio na wyświetlacz TFT.
     */
    static int pngDraw(PNGDRAW *p);

    /**
     * @brief Rysuje pełnoekranowy obraz PNG z podanej ścieżki
     * @param path Ścieżka do pliku PNG
     * @param center Czy wyśrodkować obraz
     * @return true jeśli sukces, false w przypadku błędu
     *
     * @note Funkcja statyczna - nie wymaga instancji klasy
     */
    static bool drawPngFullScreen(const char *path, bool center);

private:

    String _path;   ///< Ścieżka do pliku PNG
};

#endif // BACKGROUND_H