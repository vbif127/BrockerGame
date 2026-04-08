#include <iomanip> // Обязательно для setw
#include <iostream>
#include <map>
#include <stdlib.h>
#include <vector>

using namespace std;

/*
TODO: 123
 * Создать список валют
 * Создать генератор валют
*/

/*
 4 | #  #  #  #  #  #
 5 | #  #  #  #  #  #  #
 6 | #  #  #  #
 7 | #  #  #  #  #
 9 | #  #  #  #  #  #  #  #
   / 5  10 15 20 25 30 35 40
*/

// void draw_graph(map<int, int> history, int price_step, int period[2]) {
//   int start_price = history[period[0]];
//   cout << string(static_cast<size_t>(start_price / price_step), '#') << endl;
//   for (int i = 1; i <= period[1]; i++) {
//     start_price = history[i];
//     cout << string(static_cast<size_t>(start_price / price_step), '#') <<
//     endl;
//   }
// }

void draw_graph(map<int, int> history, int price_step, int period[2]) {
  if (price_step <= 0)
    return;

  int max_price = 0;
  size_t max_count = 0;

  // 1. Сначала находим максимум для шкалы и для выравнивания
  for (int i = period[0]; i <= period[1]; ++i) {
    if (history[i] > max_price)
      max_price = history[i];

    size_t current_count = static_cast<size_t>(history[i] / price_step);
    if (current_count > max_count)
      max_count = current_count;
  }

  // 2. Рисуем строки
  for (int i = period[0]; i <= period[1]; ++i) {
    cout << setw(2) << i << " | ";

    int current_price = history[i];
    size_t count = static_cast<size_t>(current_price / price_step);

    // Рисуем решетки
    for (size_t j = 0; j < count; ++j) {
      cout << "#   ";
    }

    // ВЫРАВНИВАНИЕ ЦЕН:
    // Добиваем пустые места пробелами до самого длинного столбца
    // (max_count - count) — сколько "ячеек" по 4 символа не дорисовано
    for (size_t space = 0; space < (max_count - count); ++space) {
      cout << "    ";
    }

    // Теперь цена всегда будет в одном и том же столбце
    cout << " (" << current_price << ")" << endl;
  }

  // 3. Разделитель и шкала
  cout << "   / ";
  for (int p = price_step; p <= max_price; p += price_step) {
    cout << left << setw(4) << p;
  }
  cout << endl;
}

class Currency {
private:
  string name;
  int price;
  vector<int> schanges;

public:
  Currency(string _name, int _price, vector<int> &_schanges) {
    name = _name;
    price = _price;
    schanges = _schanges;
  }
  void priceChange() {}
};

struct Balance {
  int price;
  int count;
};

class Terminal {
protected:
  vector<Currency> currencies;

public:
  void show_currencies() {}
};

class Broker {
private:
  map<Currency, Balance> portfolio;

public:
  void show_portfolio() {}
  void buy() {}
  void sell() {}
};

int main() {
  cout << "Hello World!\n";
  int period[2]{0, 5};
  draw_graph({{0, 100}, {1, 35}, {2, 20}, {3, 50}, {4, 105}, {5, 10}}, 10,
             period);
}
