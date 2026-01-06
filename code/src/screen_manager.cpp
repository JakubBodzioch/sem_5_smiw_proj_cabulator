#include "screen_manager.h"

// =============================================================================
// SCREEN MANAGER - Implementacja
// =============================================================================

// Definicja globalnej zmiennej stanu ekranu
// Domyślnie ustawiony na ekran powitalny (zmieniane w setup() w main.cpp)
ScreenState currentScreen = SCREEN_WELCOME;