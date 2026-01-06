Zaprojektowane urządzenie o pseudonimie roboczym Cabulator to system telematyki taksówkarskiej, 
przeznaczony dla aut prywatnych, zbudowany na platformie ESP32 z wyświetlaczem dotykowym TFT, 
łączący dane z modułu GPS i diagnostyki pojazdu OBD-II. System realizuje funkcję cyfrowego taksometru  
z rozbudowanymi możliwościami monitorowania i rejestracji przejazdu.

Główne komponenty systemu obejmują: 
▪ Czytnik GPS odpowiedzialny za śledzenie pozycji pojazdu i synchronizację czasu systemowego z danymi satelitarnymi, 
▪ Moduł OBD-II umożliwiający odczyt przebiegu i konsumpcji paliwa z pojazdu, 
▪ Kartę SD do archiwizacji tras w formacie CSV, 
▪ Wyświetlacz TFT 3,2" z interfejsem dotykowym do interakcji użytkownika.

Architektura systemu opiera się na wielowątkowym modelu RTOS z dedykowanymi taskami dla GPS i OBD, 
podczas gdy główna pętla aplikacji obsługuje logikę interfejsu użytkownika i zmianę ekranów.  
Interfejs posiada 12 ekranów funkcjonalnych w tym: ekran główny jako centralne menu, ekran aktywnej 
trasy, monitorujący dystans i koszt przejazdu, ekran ustawień taryfy (rozliczenie za km lub litr paliwa),  
panel konfiguracji jasności, ekrany diagnostyczne GPS i OBD z szczegółowymi danymi satelitów  
i parametrów pojazdu, ekran ustawień połączeń, oraz ekran informacyjny. Dane przejazdu przechowywane 
są na karcie SD w strukturze katalogów czasowych z logami i końcowymi statystykami trasy, a parametry 
taryfy są trwale zapisywane w pamięci mikroprocesora. 
