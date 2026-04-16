#include <clocale>
#include <iomanip> // Обязательно для setw
#include <iostream>
#include <map>
#include <random>
#include <stdlib.h>
#include <string>
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
                const vector<Event> &events);

int read_int(const string &message) {
  int value;

  while (true) {
    cout << message;
    if (cin >> value) {
      return value;
    }

    cout << "Некорректный ввод. Попробуйте еще раз.\n";
    cin.clear();
  }
}
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

map<int, int> build_history_map(const vector<int> &history_values) {
  map<int, int> history_map;

  for (size_t i = 0; i < history_values.size(); ++i) {
    history_map[static_cast<int>(i)] = history_values[i];
  }

  return history_map;
}

int calculate_graph_step(const vector<int> &history_values) {
  int max_value = 0;

  for (size_t i = 0; i < history_values.size(); ++i) {
    if (history_values[i] > max_value) {
      max_value = history_values[i];
    }
  }

  if (max_value <= 10) {
    return 1;
  }

  return max_value / 10;
}

void show_graph_from_history(const string &title,
                             const vector<int> &history_values, int price_step,
                             const vector<Event> &events = vector<Event>()) {
  if (history_values.empty()) {
    cout << title << "\nНет данных для построения графика.\n";
    return;
  }

  int period[2] = {0, static_cast<int>(history_values.size()) - 1};
  cout << title << "\n";
  draw_graph(build_history_map(history_values), price_step, period, events);
}

class Currency {
private:
  string name;
  int price;
  int roc;
  int price_step;
  vector<int> history;

public:
  Currency(string _name, int _price, int _price_step, int _roc) {
    name = _name;
    price = _price;
    price_step = _price_step;
    roc = _roc;
    history.push_back(price);
  }

  void priceChange() {
    static mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(-roc, roc);
    price += price_step * dist(rng);
    if (price < price_step) {
      price = price_step;
    }
    history.push_back(price);
  }

  string get_name() const { return name; }
  int get_price() const { return price; }
  vector<int> get_history() const { return history; }
  int get_price_step() const { return price_step; }
  int get_roc() const { return roc; }

  bool operator<(const Currency &other) const { return name < other.name; }
};

struct Balance {
  int price;
  int count;
};

class Broker {
private:
  map<Currency, Balance> portfolio;
  int money;
  vector<int> total_assets_history;

public:
  Broker(map<Currency, Balance> &_portfolio) {
    portfolio = _portfolio;
    money = 1000;
    total_assets_history.push_back(money);
  }

  void refresh_prices(const vector<Currency> &currencies) {
    for (size_t i = 0; i < currencies.size(); ++i) {
      map<Currency, Balance>::iterator item = portfolio.find(currencies[i]);
      if (item != portfolio.end()) {
        item->second.price = currencies[i].get_price();
      }
    }
  }

  void show_portfolio() {
    cout << "\nПортфель:\n";
    cout << "Баланс: " << money << "\n";

    bool has_assets = false;
    for (map<Currency, Balance>::iterator item = portfolio.begin();
         item != portfolio.end(); ++item) {
      if (item->second.count <= 0) {
        continue;
      }

      has_assets = true;
      cout << "- " << item->first.get_name() << ": " << item->second.count
           << " шт. по текущей цене " << item->second.price << "\n";
    }

    if (!has_assets) {
      cout << "У игрока пока нет купленных валют.\n";
    }
  }

  void show_total_assets_changes() const {
    cout << "\nИзменения стоимости портфеля:\n";

    for (size_t i = 0; i < total_assets_history.size(); ++i) {
      int current_value = total_assets_history[i];
      int delta = 0;

      if (i > 0) {
        delta = current_value - total_assets_history[i - 1];
      }

      cout << "Ход " << i << ": " << current_value;
      if (i > 0) {
        if (delta >= 0) {
          cout << " (+" << delta << ")";
        } else {
          cout << " (" << delta << ")";
        }
      }
      cout << "\n";
    }
  }

  void buy(Currency &currency, int count) {
    if (count <= 0) {
      cout << "Количество должно быть больше нуля.\n";
      return;
    }

    int total_price = currency.get_price() * count;
    if (total_price > money) {
      cout << "Недостаточно средств для покупки.\n";
      return;
    }

    Balance &balance = portfolio[currency];
    balance.price = currency.get_price();
    balance.count += count;
    money -= total_price;

    cout << "Куплено " << count << " ед. " << currency.get_name() << " за "
         << total_price << ".\n";
  }

  void sell(Currency &currency, int count) {
    if (count <= 0) {
      cout << "Количество должно быть больше нуля.\n";
      return;
    }
    map<Currency, Balance>::iterator item = portfolio.find(currency);
    if (item == portfolio.end() || item->second.count < count) {
      cout << "Недостаточно валюты для продажи.\n";
      return;
    }

    int total_price = currency.get_price() * count;
    item->second.count -= count;
    item->second.price = currency.get_price();
    money += total_price;

    cout << "Продано " << count << " ед. " << currency.get_name() << " за "
         << total_price << ".\n";
  }

  int get_total_assets() const {
    int total_assets = money;

    for (map<Currency, Balance>::const_iterator item = portfolio.begin();
         item != portfolio.end(); ++item) {
      total_assets += item->second.price * item->second.count;
    }

    return total_assets;
  }

  void record_total_assets() {
    total_assets_history.push_back(get_total_assets());
  }

  vector<int> get_total_assets_history() const { return total_assets_history; }
};

class Terminal {
protected:
  vector<Currency> currencies;
  vector<int> selected_currency_indexes;

public:
  Terminal(vector<Currency> &_currencies) { currencies = _currencies; }

  void show_currencies(vector<Currency> &_currencies) {
    cout << "\nДоступные валюты:\n";
    cout << left << setw(4) << "ID" << setw(12) << "Название" << setw(10)
         << "Цена" << setw(10) << "Шаг" << "Волат.\n";

    for (size_t i = 0; i < _currencies.size(); ++i) {
      cout << left << setw(4) << i + 1 << setw(12) << _currencies[i].get_name()
           << setw(10) << _currencies[i].get_price() << setw(10)
           << _currencies[i].get_price_step() << _currencies[i].get_roc()
           << "\n";
    }
  }

  bool is_selected_currency(int currency_index) const {
    for (size_t i = 0; i < selected_currency_indexes.size(); ++i) {
      if (selected_currency_indexes[i] == currency_index) {
        return true;
      }
    }

    return false;
  }

  void toggle_selected_currency(int currency_index) {
    for (vector<int>::iterator item = selected_currency_indexes.begin();
         item != selected_currency_indexes.end(); ++item) {
      if (*item == currency_index) {
        selected_currency_indexes.erase(item);
        cout << "Валюта удалена из выбранного списка.\n";
        return;
      }
    }

    selected_currency_indexes.push_back(currency_index);
    cout << "Валюта добавлена в выбранный список.\n";
  }

  void manage_selected_currencies() {
    cout << "\nНастройка выбранного списка валют:\n";
    show_currencies(currencies);

    if (selected_currency_indexes.empty()) {
      cout << "Список пока пуст.\n";
    } else {
      cout << "Сейчас выбраны:\n";
      for (size_t i = 0; i < selected_currency_indexes.size(); ++i) {
        cout << "- "
             << currencies[static_cast<size_t>(selected_currency_indexes[i])]
                    .get_name()
             << "\n";
      }
    }

    cout << "Введите ID валюты для добавления или удаления.\n";
    cout << "Введите 0, чтобы вернуться в меню.\n";

    while (true) {
      int currency_id = read_int("ID валюты: ");
      if (currency_id == 0) {
        break;
      }

      if (currency_id < 1 ||
          currency_id > static_cast<int>(currencies.size())) {
        cout << "Валюты с таким ID не существует.\n";
        continue;
      }

      toggle_selected_currency(currency_id - 1);
    }
  }

  void show_selected_currencies() {
    cout << "\nВыбранный список валют:\n";
    if (selected_currency_indexes.empty()) {
      cout << "Список пуст. Сначала добавьте валюты в выбранный список.\n";
      return;
    }

    for (size_t i = 0; i < selected_currency_indexes.size(); ++i) {
      Currency &currency =
          currencies[static_cast<size_t>(selected_currency_indexes[i])];

      cout << "\n" << currency.get_name() << "\n";
      cout << "Текущая цена: " << currency.get_price() << "\n";

      show_graph_from_history("Мини-график:", currency.get_history(),
                              currency.get_price_step());
    }
  }

  void show_portfolio_screen(Broker &broker) {
    broker.show_portfolio();
    broker.show_total_assets_changes();
    show_graph_from_history(
        "Мини-график стоимости портфеля:", broker.get_total_assets_history(),
        calculate_graph_step(broker.get_total_assets_history()));
  }

  void run() {
    map<Currency, Balance> start_portfolio;
    Broker broker(start_portfolio);
    const int max_turns = 10;

    cout << "Добро пожаловать в брокерскую игру!\n";
    cout << "Стартовый капитал: 1000\n";
    cout << "После каждого хода цены всех валют изменяются.\n";

    int turn = 1;
    while (turn <= max_turns) {
      cout << "\n==============================\n";
      cout << "Ход " << turn << " из " << max_turns << "\n";

      broker.refresh_prices(currencies);
      show_currencies(currencies);

      cout << "\nДействия:\n";
      cout << "1. Купить валюту\n";
      cout << "2. Продать валюту\n";
      cout << "3. Посмотреть портфель\n";
      cout << "4. Настроить выбранный список валют\n";
      cout << "5. Посмотреть выбранные валюты\n";
      cout << "6. Пропустить ход\n";
      cout << "0. Завершить игру\n";

      int action = read_int("Выберите действие: ");
      if (action == 0) {
        cout << "Игра завершена досрочно.\n";
        break;
      }

      bool is_turn_completed = false;

      if (action == 1 || action == 2) {
        int currency_id = read_int("Введите ID валюты: ");
        if (currency_id < 1 ||
            currency_id > static_cast<int>(currencies.size())) {
          cout << "Валюты с таким ID не существует.\n";
        } else {
          int count = read_int("Введите количество: ");
          Currency &selected_currency = currencies[currency_id - 1];

          if (action == 1) {
            broker.buy(selected_currency, count);
          } else {
            broker.sell(selected_currency, count);
          }

          is_turn_completed = true;
        }
      } else if (action == 3) {
        show_portfolio_screen(broker);
      } else if (action == 4) {
        manage_selected_currencies();
      } else if (action == 5) {
        show_selected_currencies();
      } else if (action == 6) {
        cout << "Ход пропущен.\n";
        is_turn_completed = true;
      } else {
        cout << "Неизвестное действие. Ход будет пропущен.\n";
        is_turn_completed = true;
      }

      if (!is_turn_completed) {
        continue;
      }

      for (size_t i = 0; i < currencies.size(); ++i) {
        currencies[i].priceChange();
      }

      broker.refresh_prices(currencies);
      broker.record_total_assets();
      ++turn;
    }

    broker.refresh_prices(currencies);
    cout << "\n==============================\n";
    cout << "Финальное состояние:\n";
    show_currencies(currencies);
    show_portfolio_screen(broker);
    cout << "Общая стоимость активов: " << broker.get_total_assets() << "\n";
  }
};

int main() {
  vector<Currency> currencies;

  setlocale(LC_ALL, "ru");

  currencies.push_back(Currency("USD", 50, 5, 2));
  currencies.push_back(Currency("EUR", 60, 5, 3));
  currencies.push_back(Currency("BTC", 120, 10, 4));
  currencies.push_back(Currency("ETH", 90, 10, 3));

  Terminal terminal(currencies);
  terminal.run();
}
