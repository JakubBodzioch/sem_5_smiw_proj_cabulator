#include "Background.h"
#include <FS.h>

int Background::s_offX = 0;
int Background::s_offY = 0;
uint16_t Background::s_lineBuf[320] = {0};
File Background::s_pngFile;
TFT_eSPI *Background::s_tft = nullptr;
PNG *Background::s_png = nullptr;

// Ustawienie ścieżki do pliku PNG
Background::Background(const String &path) : _path(path) {}
void Background::setPath(const String &path) { _path = path; }
String Background::path() const { return _path; }

// Otworzenie PNG - callbacki
void *Background::pngOpen(const char *filename, int32_t *pFileSize) {

  s_pngFile = LittleFS.open(filename, "r");
  if (!s_pngFile)
    return nullptr;
  *pFileSize = s_pngFile.size();
  return (void *)&s_pngFile; // handle, wraca w pFile->fHandle
}

// Zamknięcie PNG
void Background::pngClose(void *handle) {

  File *f = (File *)handle;
  if (f)
    f->close();
}

// Odczyt danych PNG
int32_t Background::pngRead(PNGFILE *pFile, uint8_t *pBuff, int32_t iLen) {

  File *f = (File *)pFile->fHandle;
  return f->read(pBuff, iLen);
}

// Ustawienie pozycji w pliku PNG
int32_t Background::pngSeek(PNGFILE *pFile, int32_t iPosition) {

  File *f = (File *)pFile->fHandle;
  f->seek(iPosition);
  return iPosition;
}

// DRAW jednej linii (używa s_png i s_tft)
int Background::pngDraw(PNGDRAW *p) {

  // konwersja linii PNG -> RGB565
  s_png->getLineAsRGB565(p, s_lineBuf, PNG_RGB565_BIG_ENDIAN, 0xFFFFFFFF);
  // rysowanie od x = s_offX, na wysokości s_offY + p->y, szerokość = p->iWidth
  s_tft->pushImage(s_offX, s_offY + p->y, p->iWidth, 1, s_lineBuf);
  return 1;
}

// Rysowanie PNG na pełnym ekranie
bool Background::drawPngFullScreen(const char *path, bool center) {

  // otwarcie pliku PNG
  if (s_png->open(path, pngOpen, pngClose, pngRead, pngSeek, pngDraw) != PNG_SUCCESS) {

    Serial.println("[ERROR] PNG: open/init FAIL");
    return false;
  }

  // przeliczenie offsetów do wyśrodkowania
  s_offX = s_offY = 0;
  if (center) {

    int iw = s_png->getWidth();
    int ih = s_png->getHeight();
    if (iw < s_tft->width())
      s_offX = (s_tft->width() - iw) / 2;
    if (ih < s_tft->height())
      s_offY = (s_tft->height() - ih) / 2;
  }

  // Dekodowanie i rysowanie PNG
  s_png->decode(NULL, 0);
  s_png->close();
  return true;
}

bool Background::draw(TFT_eSPI &tft, PNG &png, bool center) {
  // Ustawienie statycznych wskaźników
  s_tft = &tft;
  s_png = &png;

  // Rysowanie PNG
  return drawPngFullScreen(_path.c_str(), center);
}

// Listowanie plików w LittleFS
void Background::listFS(const char *dir) {

  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {

    Serial.println("[ERROR][FS] root is not a directory or failed to open");
    return;
  }

  int fileCount = 0;
  File file = root.openNextFile();
  
  while (file) {

    Serial.printf("%s (%uB)\n", file.name(), (unsigned)file.size());
    fileCount++;
    file = root.openNextFile();
  }
  Serial.printf("[FS] Found %d files in FS root\n", fileCount);
}
