#include <iostream>
#include <vector>

#include <chrono>
#include <thread>

using namespace std;

const int DECK_SIZE = 10;

enum CellState { // состояние ячейки
    EMPTY,
    SHIP,
    HIT,
    MISS
};

class Ship {
public:
    int size;
    vector<pair<int, int>> coordinates;
    Ship(int s) : size(s) {}

    void addCoordinates(int x, int y) {
        coordinates.push_back(make_pair(x, y));
    }

    bool isSunk(const vector<vector<CellState>>& setka) const {
        for (const auto& coord : coordinates) {
            if (setka[coord.first][coord.second] != HIT) {
                return false;
            }
        }
        return true;
    }
};

class Deck {
private:
    vector<vector<CellState>> setka;

    void markAroundSunkShip(const Ship& ship) {
        for (const auto& coord : ship.coordinates) {
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int newX = coord.first + i;
                    int newY = coord.second + j;
                    if (newX >= 0 && newX < DECK_SIZE && newY >= 0 && newY < DECK_SIZE) {
                        if (setka[newX][newY] == EMPTY) {
                            setka[newX][newY] = MISS;
                        }
                    }
                }
            }
        }
    }

public:
    Deck() : setka(DECK_SIZE, vector<CellState>(DECK_SIZE, EMPTY)) {}

    void display(bool hideShips = false) {
        cout << "+ ";
        for (int i = 0; i < DECK_SIZE; ++i)
            cout << i << " ";
        cout << endl;

        for (int i = 0; i < DECK_SIZE; ++i) {
            cout << i << " ";
            for (int j = 0; j < DECK_SIZE; ++j) {
                if (setka[i][j] == EMPTY)
                    cout << ". ";
                else if (setka[i][j] == SHIP)
                    cout << (hideShips ? ". " : "П ");
                else if (setka[i][j] == HIT)
                    cout << "X ";
                else if (setka[i][j] == MISS)
                    cout << "* ";
            }
            cout << endl;
        }
    }

    bool canPlaceShip(int x, int y, int size, bool horizontal) {
        if (horizontal) {
            if (y + size > DECK_SIZE) return false;
            for (int i = 0; i < size; i++) {
                if (!isValidPlacement(x, y + i)) return false;
            }
        }
        else {
            if (x + size > DECK_SIZE) return false;
            for (int i = 0; i < size; i++) {
                if (!isValidPlacement(x + i, y)) return false;
            }
        }
        return true;
    }

    bool isValidPlacement(int x, int y) {
        if (setka[x][y] != EMPTY) return false;

        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int newX = x + i;
                int newY = y + j;
                if (newX >= 0 && newX < DECK_SIZE && newY >= 0 && newY < DECK_SIZE) {
                    if (setka[newX][newY] == SHIP) return false;
                }
            }
        }
        return true;
    }

    void placeShip(Ship& ship, int x, int y, bool horizontal) {
        for (int i = 0; i < ship.size; ++i) {
            if (horizontal) {
                setka[x][y + i] = SHIP;
                ship.addCoordinates(x, y + i);
            }
            else {
                setka[x + i][y] = SHIP;
                ship.addCoordinates(x + i, y);
            }
        }
    }

    bool shoot(int x, int y, Ship& hitShip) {
        if (setka[x][y] == SHIP) {
            setka[x][y] = HIT;
            for (Ship& ship : ships) {
                if (ship.coordinates.size() > 0 && setka[ship.coordinates[0].first][ship.coordinates[0].second] == HIT) {
                    if (ship.isSunk(setka)) {
                        markAroundSunkShip(ship);
                    }
                }
            }
            return true;
        }
        else if (setka[x][y] == EMPTY) {
            setka[x][y] = MISS;
            return false;
        }
        return false;
    }

    bool Loose() {
        for (int i = 0; i < DECK_SIZE; ++i) {
            for (int j = 0; j < DECK_SIZE; ++j) {
                if (setka[i][j] == SHIP) return false;
            }
        }
        return true;
    }

    void reset() {
        setka.assign(DECK_SIZE, vector<CellState>(DECK_SIZE, EMPTY));
        ships.clear();
    }

    vector<Ship> ships;
};

class Player {
public:
    string name;
    Deck deck;

    Player(string n) : name(n) {}

    void placeShips() {
        deck.ships.push_back(Ship(4));
        deck.ships.push_back(Ship(3));
        deck.ships.push_back(Ship(3));
        deck.ships.push_back(Ship(2));
        deck.ships.push_back(Ship(2));
        deck.ships.push_back(Ship(2));
        deck.ships.push_back(Ship(1));
        deck.ships.push_back(Ship(1));
        deck.ships.push_back(Ship(1));
        deck.ships.push_back(Ship(1));

        for (Ship& ship : deck.ships) {
            bool placed = false;
            while (!placed) {
                int x = rand() % DECK_SIZE;
                int y = rand() % DECK_SIZE;
                bool horizontal = rand() % 2;
                if (deck.canPlaceShip(x, y, ship.size, horizontal)) {
                    deck.placeShip(ship, x, y, horizontal);
                    placed = true;
                }
            }
        }
    }

    void resetShips() {
        deck.reset();
        placeShips();
    }

    bool shoot(Player& enemy, int x, int y) {
        Ship dummyShip(0);
        return enemy.deck.shoot(x, y, dummyShip);
    }
};

class Game {
private:
    Player player1;
    Player player2;
    bool humanVsComputer;

    void delay() {
        // Задержка в 0.1 секунды для компьютера перед выстрелом
        this_thread::sleep_for(chrono::milliseconds(200));
    }

    void displayDecks() {
        cout << player1.name << "'s Deck: " << endl;
        player1.deck.display();
        cout << endl;

        cout << player2.name << "'s Deck: " << endl;
        player2.deck.display(true);
        cout << endl;
    }

public:
    Game(string name1, string name2, bool hvsc) : player1(name1), player2(name2), humanVsComputer(hvsc) {}

    void start() {
        srand(time(0));

        player1.placeShips();
        player2.placeShips();

        bool player1Turn = true;

        while (!player1.deck.Loose() && !player2.deck.Loose()) {
            displayDecks();

            Player& currentPlayer = player1Turn ? player1 : player2;
            Player& enemy = player1Turn ? player2 : player1;

            int x, y;
            bool hit = false;

            do {
                if (humanVsComputer && player1Turn) {
                    cout << currentPlayer.name << "'s turn. Enter coordinates to shoot (x y): ";
                    cin >> x >> y;

                    while (x < 0 || x >= DECK_SIZE || y < 0 || y >= DECK_SIZE) {
                        cout << "Invalid coordinates. Enter coordinates to shoot (x y): ";
                        cin >> x >> y;
                    }
                }
                else {
                    cout << currentPlayer.name << "'s turn (random shot)." << endl;
                    delay(); // Задержка перед выстрелом компьютера
                    x = rand() % DECK_SIZE;
                    y = rand() % DECK_SIZE;
                }

                hit = currentPlayer.shoot(enemy, x, y);
                displayDecks(); // Отображаем измененную карту после каждого выстрела

                if (hit) {
                    cout << "Hit!!!" << endl;
                }
                else {
                    cout << "Miss:((((" << endl;
                }
            } while (hit); // Цикл продолжается, пока игрок попадает

            player1Turn = !player1Turn;
        }

        if (player1.deck.Loose()) {
            cout << player2.name << " wins!!!" << endl;
        }
        else {
            cout << player1.name << " wins!!!" << endl;
        }
    }
};

int main() {
    setlocale(LC_ALL, "ru");
    int typeGame;
    cout << "1. Human VS Computer" << endl;
    cout << "2. Computer VS Computer" << endl;
    cin >> typeGame;

    bool humanVsComputer = (typeGame == 1);

    Game game("Jack Sparrow", humanVsComputer ? "Computer" : "Blackbeard", humanVsComputer);
    game.start();

    return 0;
}


/// 1. добавить обводку уничтоженого корабля = DONE!
/// 2. В случае попадания сделать следующи ход = DONE!
/// 3. Отступы между кораблями при расстановке = DONE!
/// 4. Игра ИИ против ИИ = DONE!
/// 5. Сохранение прогресса и продолжение игры




