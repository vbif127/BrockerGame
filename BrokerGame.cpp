#include <cmath>   // для round
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

// events: [номер_периода][точная_цена] = 'B' или 'S'
struct Event {
  int time;  // Номер периода (строка)
  int price; // Точная цена
  char type; // 'B' (Buy) или 'S' (Sell)
};

void draw_graph(map<int, int> history, int price_step, int period[2],
                const vector<Event> &events) {
  if (price_step <= 0)
    return;

  int max_price = 0;
  // 1. Поиск максимумов (с правильным округлением вверх)
  for (int i = period[0]; i <= period[1]; ++i) {
    if (history[i] > max_price)
      max_price = history[i];
  }

  // Считаем общее макс. кол-во ячеек для всей шкалы
  size_t max_count =
      static_cast<size_t>((max_price + price_step - 1) / price_step);

  // 2. Отрисовка строк
  for (int i = period[0]; i <= period[1]; ++i) {
    cout << setw(2) << i << " | ";

    int current_line_price = history[i];
    // Важно: здесь тоже округляем вверх
    size_t cells_in_line =
        static_cast<size_t>((current_line_price + price_step - 1) / price_step);

    int event_price_to_show = -1;

    for (size_t j = 1; j <= cells_in_line; ++j) {
      int cell_min = static_cast<int>(j - 1) * price_step;
      int cell_max = static_cast<int>(j) * price_step;
      char symbol = '#';

      for (size_t k = 0; k < events.size(); ++k) {
        if (events[k].time == i && events[k].price > cell_min &&
            events[k].price <= cell_max) {
          symbol = events[k].type;
          event_price_to_show = events[k].price;
          break;
        }
      }
      cout << symbol << "   ";
    }

    // 3. Выравнивание (теперь max_count всегда >= cells_in_line)
    for (size_t space = 0; space < (max_count - cells_in_line); ++space) {
      cout << "    ";
    }

    cout << " (" << setw(2) << current_line_price << ")";
    if (event_price_to_show != -1) {
      cout << " [" << event_price_to_show << "]";
    }
    cout << endl;
  }

  // 4. Шкала (Ось X)
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
  int period[2]{1, 4};
  draw_graph({{0, 25}, {1, 35}, {2, 20}, {3, 50}, {4, 15}}, 8, period,
             {{1, 32, 'B'}, {3, 50, 'S'}});
}
