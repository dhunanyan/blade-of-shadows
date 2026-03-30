# Katalog logs - Wyniki analizy kodu

Ten katalog zawiera wyniki różnych narzędzi do analizy kodu C++. Narzędzia te pomagają wykryć błędy, problemy z wydajnością, bezpieczeństwem i stylem kodu.

## ⚠️ Ważne informacje

**Wszystkie narzędzia poza kompilatorem i Valgrind mogą się mylić!** Mogą zgłaszać fałszywe alarmy (false positives) lub nie wykryć rzeczywistych problemów (false negatives). Traktuj ich wyniki jako wskazówki, a nie ostateczne werdykty.

Więcej informacji o narzędziach: [Bezpłatne narzędzia ułatwiające wykrywanie błędów w programach C/C++](https://cpp0x.pl/artykuly/Konfiguracja/Narzedzia-dla-programistow-C++/Bezplatne-narzedzia-ulatwiajace-wykrywanie-bledow-w-programach-C-C++/89)

---

## 📋 Opis plików (w kolejności wykonywania)

### 1️⃣ Budowanie projektu

#### `01_cmake.log`
**Narzędzie:** [CMake](https://cmake.org/)  
**Przeznaczenie:** Konfiguracja systemu budowania projektu  
**Co zawiera:** Informacje o wykrytych kompilatorach, bibliotekach i konfiguracji projektu

#### `02_makefile.log`
**Narzędzie:** [Make](https://www.gnu.org/software/make/) + kompilator (g++/clang++)  
**Przeznaczenie:** Kompilacja kodu źródłowego  
**Co zawiera:** Komunikaty kompilatora, ostrzeżenia i błędy kompilacji

---

### 2️⃣ Testy jednostkowe

#### `03A_testy_tekstowo.log`
**Narzędzie:** [Google Test](https://github.com/google/googletest)  
**Przeznaczenie:** Wykonanie testów jednostkowych (format tekstowy)  
**Co zawiera:** Wyniki testów w formacie czytelnym dla człowieka

#### `03B_testy_json.json`
**Narzędzie:** [Google Test](https://github.com/google/googletest)  
**Przeznaczenie:** Wykonanie testów jednostkowych (format JSON)  
**Co zawiera:** Wyniki testów w formacie JSON do automatycznej analizy

---

### 3️⃣ Analiza pamięci (BARDZO WAŻNE!)

#### `04_valgrind.log`
**Narzędzie:** [Valgrind](https://valgrind.org/)  
**Przeznaczenie:** Log z uruchomienia Valgrind  
**Co zawiera:** Informacje o procesie uruchamiania analizy pamięci

#### `04A_valgrind_testy.log`
**Narzędzie:** [Valgrind](https://valgrind.org/)  
**Przeznaczenie:** Wykrywanie wycieków pamięci i błędów dostępu do pamięci  
**Co zawiera:** 
- Wycieki pamięci (memory leaks)
- Nieprawidłowe odczyty/zapisy (invalid read/write)
- Użycie niezainicjalizowanej pamięci
- Podwójne zwolnienie pamięci (double free)

**⚠️ To narzędzie jest bardzo wiarygodne - każdy błąd wykryty przez Valgrind to prawdziwy problem!**

---

### 4️⃣ Statyczna analiza kodu

#### `05_cppcheck.log`
**Narzędzie:** [Cppcheck](http://cppcheck.sourceforge.net/)  
**Przeznaczenie:** Log z uruchomienia Cppcheck  
**Co zawiera:** Informacje o procesie analizy

#### `05A_cppcheck_raport.xml`
**Narzędzie:** [Cppcheck](http://cppcheck.sourceforge.net/)  
**Przeznaczenie:** Statyczna analiza kodu (format XML)  
**Co zawiera:** Potencjalne błędy, problemy z wydajnością, nieużywane funkcje

#### `05B_cppcheck_raport.txt`
**Narzędzie:** [Cppcheck](http://cppcheck.sourceforge.net/)  
**Przeznaczenie:** Statyczna analiza kodu (format tekstowy)  
**Co zawiera:** To samo co XML, ale w formacie czytelnym dla człowieka

**⚠️ Może zgłaszać fałszywe alarmy - sprawdź każde ostrzeżenie!**

---

### 5️⃣ Analiza bezpieczeństwa

#### `06_flawfinder.log`
**Narzędzie:** [Flawfinder](https://dwheeler.com/flawfinder/)  
**Przeznaczenie:** Log z uruchomienia Flawfinder  
**Co zawiera:** Informacje o procesie analizy bezpieczeństwa

#### `06A_flawfinder_raport.html`
**Narzędzie:** [Flawfinder](https://dwheeler.com/flawfinder/)  
**Przeznaczenie:** Wykrywanie potencjalnych luk bezpieczeństwa (format HTML)  
**Co zawiera:** Niebezpieczne funkcje (np. `strcpy`, `sprintf`), możliwe przepełnienia bufora

#### `06B_flawfinder_raport.txt`
**Narzędzie:** [Flawfinder](https://dwheeler.com/flawfinder/)  
**Przeznaczenie:** Wykrywanie potencjalnych luk bezpieczeństwa (format tekstowy)  
**Co zawiera:** To samo co HTML, ale w formacie tekstowym

**⚠️ Często zgłasza fałszywe alarmy - oceniaj krytycznie!**

---

### 6️⃣ Analiza wydajności

#### `07_perf.log`
**Narzędzie:** [perf](https://perf.wiki.kernel.org/)  
**Przeznaczenie:** Log z uruchomienia analizy wydajności  
**Co zawiera:** Informacje o procesie profilowania

#### `07A_perf_statystyki.txt`
**Narzędzie:** [perf](https://perf.wiki.kernel.org/)  
**Przeznaczenie:** Statystyki wydajności programu  
**Co zawiera:** 
- Liczba cykli procesora
- Liczba instrukcji
- Trafienia/chybienia cache
- Przewidywania skoków (branch predictions)

#### `07B_perf_script.txt`
**Narzędzie:** [perf](https://perf.wiki.kernel.org/)  
**Przeznaczenie:** Surowe dane profilowania  
**Co zawiera:** Szczegółowe informacje o próbkach wydajności

#### `07C_perf_script_folded.txt`
**Narzędzie:** [perf](https://perf.wiki.kernel.org/) + [FlameGraph](https://github.com/brendangregg/FlameGraph)  
**Przeznaczenie:** Dane przygotowane do wizualizacji  
**Co zawiera:** Zagregowane stosy wywołań funkcji

#### `07D_perf_flamegraph.svg`
**Narzędzie:** [FlameGraph](https://github.com/brendangregg/FlameGraph)  
**Przeznaczenie:** Wizualizacja wydajności programu  
**Co zawiera:** Interaktywny wykres pokazujący, które funkcje zużywają najwięcej czasu procesora

**💡 Otwórz plik SVG w przeglądarce, aby zobaczyć interaktywny wykres!**

---

### 7️⃣ Analiza stylu kodu

#### `08_cpplint.log`
**Narzędzie:** [cpplint](https://github.com/cpplint/cpplint)  
**Przeznaczenie:** Log z uruchomienia cpplint  
**Co zawiera:** Informacje o procesie analizy stylu

#### `08A_cpplint_raport.txt`
**Narzędzie:** [cpplint](https://github.com/cpplint/cpplint)  
**Przeznaczenie:** Sprawdzanie zgodności ze stylem Google C++  
**Co zawiera:** 
- Problemy z formatowaniem
- Białe znaki na końcu linii
- Użycie tabulatorów zamiast spacji
- Problemy z nagłówkami

**⚠️ Bardzo rygorystyczne - nie wszystkie sugestie są konieczne do poprawy!**

---

### 8️⃣ Zaawansowana analiza statyczna

#### `09_clang_tidy.log`
**Narzędzie:** [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)  
**Przeznaczenie:** Log z uruchomienia clang-tidy  
**Co zawiera:** Informacje o procesie zaawansowanej analizy

#### `09A_clang_tidy_raport.txt`
**Narzędzie:** [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)  
**Przeznaczenie:** Zaawansowana analiza statyczna i modernizacja kodu  
**Co zawiera:** 
- Sugestie modernizacji kodu (np. użycie `auto`, range-based for)
- Problemy z czytelnością
- Potencjalne błędy
- Problemy z wydajnością

#### `09B_clang_tidy_poprawki.yml`
**Narzędzie:** [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)  
**Przeznaczenie:** Automatyczne poprawki w formacie YAML  
**Co zawiera:** Sugerowane zmiany, które można automatycznie zastosować

**⚠️ Może sugerować zbyt wiele zmian - wybieraj mądrze!**

---

### 9️⃣ Dokumentacja

#### `10_dokumentacja.pdf`
**Narzędzie:** [Doxygen](https://www.doxygen.nl/) + pdflatex  
**Przeznaczenie:** Dokumentacja kodu wygenerowana z komentarzy  
**Co zawiera:** 
- Lista zadań do zrobienia (TODO)
- Dokumentacja funkcji i klas
- Diagramy zależności (jeśli dostępny Graphviz)

---

## 🎯 Jak korzystać z tych narzędzi?

1. **Zacznij od kompilacji** (`01_cmake.log`, `02_makefile.log`) - napraw wszystkie błędy kompilatora
2. **Uruchom testy** (`03A_testy_tekstowo.log`) - upewnij się, że wszystkie testy przechodzą
3. **Sprawdź Valgrind** (`04A_valgrind_testy.log`) - **TO NAJWAŻNIEJSZE!** Napraw wszystkie wycieki pamięci
4. **Przejrzyj analizy statyczne** (`05A_cppcheck_raport.txt`, `06B_flawfinder_raport.txt`) - oceń, które ostrzeżenia są istotne
5. **Sprawdź styl kodu** (`08A_cpplint_raport.txt`) - popraw oczywiste problemy ze stylem
6. **Rozważ sugestie clang-tidy** (`09A_clang_tidy_raport.txt`) - wybierz sensowne modernizacje
7. **Sprawdź dokumentację** (`10_dokumentacja.pdf`) - upewnij się, że wykonałeś wszystkie TODO

---

## 📚 Dodatkowe zasoby

- [Artykuł o narzędziach do wykrywania błędów](https://cpp0x.pl/artykuly/Konfiguracja/Narzedzia-dla-programistow-C++/Bezplatne-narzedzia-ulatwiajace-wykrywanie-bledow-w-programach-C-C++/89)
- [Valgrind Quick Start Guide](https://valgrind.org/docs/manual/quick-start.html)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Modern C++ Best Practices](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)

---

**Powodzenia w nauce programowania! 🚀**
