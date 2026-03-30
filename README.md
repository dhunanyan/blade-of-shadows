# Dziedziczenie i interakcje między obiektami w formie gierki
W zadaniu chodzi o to, aby poćwiczyć używanie klas, ale też stosowanie dziedziczenia w programowaniu. Poza tym też interakcje między obiektami - a wszystko na przykładzie gierki 2D.

**Uwaga:** W przedstawiony sposób się nie robi gier. W tego typu grach stosuje się [Entity component system](https://en.wikipedia.org/wiki/Entity_component_system). W dalszym etapie opisu uwagi od osoby znającej się na GameDevie.

### Utrudnienie - makra preprocesora
Zadanie zawiera testy automatyczne, które testują czy dana metoda jest zaimplementowana poprawnie. Jeśli metody nie ma a byłaby testowana to byłby błąd kompilacji. Tym samym zadanie można by oddać wyłącznie jako wszystko lub jako nic. Dlatego też korzystam z makr preprocesora, które zależnie od tego czy dana funkcjonalność jest zaimplementowana czy nie aktywują (w trakcie kompilacji) odpowiednie części kodu.


Treść do wykonania:
---------
### Klasa `Object`:
Proszę o zaimplementowanie klasy `Object`, która będzie miała trzymała typ obiektu (zdefiniowany `ObjectType`), oraz pozycje (zdefiniowany `Position`).
Zaimplementuj metody:
- Konstruktor przyjmujący `ObjectType` i `Position` (pozycja niech ma wartość domyślną)
- gettery `type()` i `position()`
- następnie można usunąć: `UNIMPLEMENTED_objectWithConstructorAndGetters`
- setter `setPosition`, następnie można usunąć `UNIMPLEMENTED_objectSetPosition`
- metody do poruszania obiektem o jedną komórkę: `moveUp()`, `moveDown()`, `moveLeft()`, `moveRight()`, następnie można usunąć `UNIMPLEMENTED_movingObject`
   1. **Pytanie na myślenie:** co zyskali byśmy gdyby `moveX()` zwracało `*this`?

### Klasa `Player` (dziedzicząca po `Object`):
Proszę o utworzenie klasy `Player`, która będzie dziedziczyć po klasie `Object`, następnie proszę zakomentować `UNIMPLEMENTED_playerIsObject`
- Klasa ta powinna mieć mieć składową `direction_`, a także operujący na niej getter i setter. Domyślna wartość to `Direction::RIGHT`
   - po zdefiniowaniu proszę usunąć `UNIMPLEMENTED_playerHasDirection`
- Klasa powinna mieć składową oznaczającą czy gracz żyje, oraz getter i setter: `isAlive()`, `setIsAlive()`
   - po zdefiniowaniu proszę usunąć `UNIMPLEMENTED_playerCanDie`

### Klasa `Bullet` (dziedzicząca po `Object`):
Proszę o utworzenie klasy `Bullet`, która będzie dziedziczyć po klasie `Object`, następnie proszę zakomentować `UNIMPLEMENTED_bulletIsObject`
- Klasa ta powinna mieć mieć składową `direction_`, a także operujący na niej getter i setter
- Klasa powinna mieć konstruktor przyjmujący kierunek i pozycje początkową
- Po zaimplementowania tego proszę zakomentować `UNIMPLEMENTED_bulletHasDirectionAndPosition`
- Klasa powinna zawierać statyczny atrybut odpowiadający prędkości o wartości `2`, oraz getter `speed()`

### Klasa `Enemy` (dziedzicząca po `Object`):
Proszę o utworzenie klasy `Enemy`, która będzie dziedziczyć po klasie `Object`
- Klasa ta powinna mieć mieć składową odpowiadającą ilości punktów życia, oraz getter: `lifePercent()` (**W PROCENTACH!**)
- Klasa powinna mieć konstruktor przyjmujący aktualną pozycje oraz maksymalne życie z wartością domyślną.
- Klasa powinna mieć metodę `isAlive()` która zwraca prawdę jeśli wróg ma życie powyżej zera
- Klasa powinna mieć metodę `decreaseLife()` przyjmującą jako argument liczbę obrażeń
- Po zaimplementowaniu powyższego proszę zakomentować makro `UNIMPLEMENTED_enemy`

### Klasa Engine - główny silnik gry:
Proszę o zaimplementowanie klasy `Engine`, która będzie zarządzać obiektami, oraz będzie interfejsem do komunikacji z GUI.
- Szczegółowy opis poszczególnych metod znajduje się nad metodami.
- Nie są napisane testy dla całej klasy, więc możliwe jest przejście wszystkich testów, ale gra nie będzie działać.
- Można otrzymać punkty z aktywności za te zajęcia za uruchomienie gry i pokazanie mi (tylko osoby, które nie mają jeszcze maksa), to na następny raz
- Oto metody do zaimplementowania z pliku `engine.h`:
```c++
class Engine
{
public:
    /// Konstruktor, który dokona odpowiednich inicjalizacji
    Engine(std::size_t stageWidth, std::size_t stageHeight);

    /// Destruktor (jeśli trzeba)
    ~Engine();

    /// metody zwracające informacje o rozmiarze planszy
    std::size_t stageWidthCells() const;
    std::size_t stageHeightCells() const;

    /// zakomentowac makro: UNIMPLEMENTED_engineConstructedWithWidthAndHeight

    /// metody zwracające informacje na temat gracza
    Position playerPosition() const; /// domyslnie srodek planszy
    Direction playerDirection() const; /// domyślnie Direction::RIGHT
    bool isPlayerAlive() const;

    /// zakomentowac makro: UNIMPLEMENTED_engineHasGettersToGetInformationAboutPlayer

    /// metody obsługiwane z klawiatury
    void playerShoots();
    void movePlayerUp();
    void movePlayerDown();
    void movePlayerLeft();
    void movePlayerRight();

    /// zakomentowac makro: UNIMPLEMENTED_engineCanMovePlayer

    /// metody zwracające informacje na temat obiektów w grze
    const std::vector<Bullet>& bullets() const
    {
        return bullets_;
    }

    /// zakomentowac makro: UNIMPLEMENTED_engineHandlePlayerShooting

    /// metoda wywoływania kilka razy na sekundę, która ma za zadanie zaktualizowanie obiektów
    void update();

    const auto& enemies() const
    {
        return enemies_;
    }

protected:
    /// metoda, która doda określoną liczbę wrogów w miejscach zwróconych przez generator:
    void randEnemies(Position (*positionGenerator)(int, int)=generateNewEnemyPosition);
    /// metoda, która wpierw usunie zabitych wrogów, a następnie wykona ruch
    void updateEnemies();

    /// zakomentuj makro: UNIMPLEMENTED_engineHandlesEnemies

    /// metoda, która powinna zaktualizować pociski, czyli przesunąć je,
    /// obsłużyć trafienie wroga lub wyjście poza scenę
    void updateBullets();
```

### Opcjonalna klasa Stage:
Tej klasy użyłem aby przypisywać obiekty do sceny, tym samym łatwiej jest sprawdzić czy dana komórka jest zajęta, oraz przez kogo.
Jednakże można sobie poradzić bez tej klasy.

____________________________________________________________________________________
Informacje jak czytać testy znajdują się w materiale [wideo](https://banbye.com/watch/v_5z_XAxtzSUN-), a w nagraniu:
1. Uruchomienie gry, ale jest ona niekompletna, **nie jestem specjalistą od gier**
2. Przegląd obiektów i ich interakcja.
3. Przełączanie między projektami i biblioteka QT.
4. Omówienie testów, które są i których nie ma.
6. Ciekawostki związane ze składnią:
   - różnice w includowaniu `""` i `<>`
   - forward declaration vs include
   - enum class
   - aliasy typów np. `uint32_t` z `#include <cstdint>`
   - zasłanianie składowych przy dziedziczeniu
   - wskaźniki do funkcji
7. Przegląd zasobów (można podmienić zasoby)
8. Omówienie QT GUI

____________________________________________________________________________________
Po implementowaniu powyższych poleceń i komentowania makr powinno  doprowadzać do sytuacji, że coraz więcej testów przechodzi.

## Uwaga:
Sugeruję pisać porządnie, czyli deklaracje metod w pliku nagłówkowym, definicje w źródłowym
w argumentach funkcji przez referencję, proszę też stosować słówko `const` w odpowiednich miejscach.
Metody wspólne dla wszystkich klas jako statyczne. Jeśli zwracamy stałą czasu kompilacji aby była to metoda `constexpr`.

[Bardziej szczegółowe informacje jak pisać programy w ładnym stylu](https://programowaniec.wordpress.com/2017/11/09/czego-sie-czepiam/) dla zaawansowanych.
____________________________________________________________________________________
## Ocenianie:
1. Ocenia [Bobot](https://gitlab.com/agh-courses/bobot), na ten moment w następujący sposób:
   1. Kompilacja nadesłanego rozwiązania - bez tego zero punktów. Bobot pracuje na Linuxie, używa kompilatora `g++`.
   2. Uruchamianie testów - za każdy test, który przejdzie są punkty, ale mogą być odjęte w kolejnych krokach.
   3. Jeśli program się wywala na którymś z testów (to się pojawia często u osób pracujących na Windowsie - ten system pozwala pisać po nie-swojej pamięci, Linux nie pozwala) lub jest timeout - wtedy będzie przyznane tyle punktów ile przechodzi testów **minus karne punkty**.
   4. Jest odpalane narzędzie [valgrind](https://valgrind.org/), które sprawdza czy umiemy obsługiwać pamięć w praktyce - jeśli nie to **minus punkt**.
   5. Odpalane są też inne narzędzia takie jak [cppcheck](http://cppcheck.net/), czy [fawfinde](https://dwheeler.com/flawfinder/) i inne. One nie odejmują punktów, no ale mają pomóc w pisaniu porządnych programów. Nie olewajmy tego.
   6. Antyplagiat - za wykrycie plagiatu (jest specjalne narzędzie) otrzymuje się 0 punktów. Wysyłajmy więc swoje!
____________________________________________________________________________________
## Najczestsze pytania/błędy/problemy:
1. Zdefiniowałem jedną z klas dziedziczących po `Object`, pojawia się błąd komunikat kompilacji 
   mówiący o konieczności zawołania konstruktora klasy Object z argumentem.
   - Trzeba go zawołać na liście inicjalizacyjnej konstruktora np. KlasaPochodnaPoObject(): Object(ObjectType::OBJECT_UNKNOWN)
2. Coś dziwnie działa/wywala się nie wiadomo na czym
    - W tej sytuacji najlepiej jest użyć debuggera, polecam zapoznać się z [krótkim filmikiem](https://www.youtube.com/watch?v=5wGsRdumueU&list=PLQ176FUIyIUawCufBKmmWkRq2t5sl1xsM) informującym jak się debugguje w CLion. Dzięki temu można prześledzić wykonywanie programu linijka w linijkę, instrukcja za instrukcją i znaleźć błąd samodzielnie.

____________________________________________________________________________________
# Pytania po implementacji ćwiczenia:
1. Czego ważnego dla mnie się dzisiaj nauczyłem/nauczyłam?

____________________________________________________________________________________
# Jak skonfigurować sobie pracę nad paczką:
W formie [wideo](https://banbye.com/watch/v_i79PoGIWrjRC) - jest to instrukcja dla innej paczki (z innego kierunku), ale kroki są analogiczne.
Również jak przy pomocy CLiona obslugiwać gita i gitlaba ([wideo](https://banbye.com/watch/v_nNPin8m9LKqn)).

**Alternatywnie poniżej jest to spisane w kolejnej sekcji**

____________________________________________________________________________________
# Uwagi od specjalisty od GameDev:
W taki sposób się nie robi gier 2D! Lepiej studentom tego nie dawać, bo wyrobią sobie złe nawyki, zaczął bym poprawki od:
1. wywalić grida (Stage), aby ruch był płynny, czyli pozycje na floatach
2. zamiast QT coś lepszego np.: cocos2d lub jeszcze lepiej SFML
3. direction -> Vector2f
4. speed np. w m/s
5. kompozycja nad dziedziczeniem
6. nie robić typów w enumach, lepiej dynamic_casty
7. wywalić klasę `Object` - to jest strasznie wolne
8. po wywaleniu sceny zrobić zmianę nazwy `Engine` -> `Scene`
9. `std::vector<std::shared_ptr<Object>> objects_;` w `Stage` -> oddzielne tablice dla wrogów i pocisków
10. wywalić `shared_ptr` i lepiej też `unique_ptr`


____________________________________________________________________________________
## Grading (section copied from Mateusz Ślażyński, of course he agreed):

* [ ] Make sure, you have a **private** group
  * [how to create a group](https://docs.gitlab.com/ee/user/group/#create-a-group)
* [ ] Fork this project into your private group
  * [how to create a fork](https://docs.gitlab.com/ee/user/project/repository/forking_workflow.html#creating-a-fork)
* [ ] Add @bobot-is-a-bot as the new project's member (role: **maintainer**)
  * [how to add an user](https://docs.gitlab.com/ee/user/project/members/index.html#add-a-user)

## How To Submit Solutions

1. [ ] Clone repository: `git clone` (clone only once the same repository):

    ```bash
    git clone <repository url>
    ```
2. [ ] Solve the exercises
3. [ ] Commit your changes

    ```bash
    git add <path to the changed files>
    git commit -m <commit message>
    ```
4. [ ] Push changes to the gitlab main branch

    ```bash
    git push -u origin main
    ```

The rest will be taken care of automatically. You can check the `GRADE.md` file for your grade / test results. Be aware that it may take some time (up to one hour) till this file. Details can be found in `./logs/` directory where You can check compilation results, tests logs etc.

## Project Structure

      .
      ├── CMakeLists.txt        # CMake configuration file - the file is to open out project in our IDE
      ├── enemy.h
      ├── enemy.cpp
      ├── engine.h
      ├── engine.cpp
      ├── direction.h
      ├── Dockerfile
      ├── game                  # inner CMake for game in QT (qt libraries must be installed)
      │   ├── background.png
      │   ├── bibliography.txt
      │   ├── bullet.png
      │   ├── CMakeLists.txt
      │   ├── enemy.png
      │   ├── gameMain.cpp
      │   ├── mainwindow.cpp
      │   ├── mainwindow.h
      │   ├── mainwindow.ui
      │   ├── music.mp3
      │   ├── player.png
      │   └── resources.qrc
      ├── main.cpp              # main file - here we can test out solution manually, but it is not required
      ├── object.h
      ├── object.cpp
      ├── player.h
      ├── position.h
      ├── bullet.h
      ├── stage.h
      ├── stage.cpp
      ├── README.md             # this file
      └── tests                 # here are tests for exercise, inner CMakeLists.txt, GTest library used by tests
          ├── CMakeLists.txt    # inner CMake for tests - it is included by outter CMake
          ├── engineTests.cpp
          ├── objectTests.cpp
          ├── playerTests.cpp
          ├── bulletTests.cpp
          └── lib               # directory containing GTest library
