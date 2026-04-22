#include <clocale>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

/*
TODO: 123
 * Create a currency list
 * Create a currency generator
*/

/*
 4 | #  #  #  #  #  #
 5 | #  #  #  #  #  #  #
 6 | #  #  #  #
 7 | #  #  #  #  #
 9 | #  #  #  #  #  #  #  #
   / 5  10 15 20 25 30 35 40
*/

// events: [time_period][exact_price] = 'B' or 'S'
struct Event {
  int time;  // Period number (row)
  int price; // Exact price
  int count; // Number of units
  char type; // 'B' (Buy) or 'S' (Sell)
};

int read_int(const string &message) {
  int value;

  while (true) {
    cout << message;
    if (cin >> value) {
      return value;
    }

    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Invalid input. Try again.\n";
  }
}

void draw_graph(map<int, int> history, int price_step, int period[2],
                const vector<Event> &events) {
  if (price_step <= 0)
    return;
  if (price_step < 5)
    price_step = 5;

  int max_price = 0;
  // 1. Find max values with upward rounding
  for (int i = period[0]; i <= period[1]; ++i) {
    if (history[i] > max_price)
      max_price = history[i];
  }

  // Calculate the max number of cells for the full scale
  size_t max_count =
      static_cast<size_t>((max_price + price_step - 1) / price_step);

  // 2. Draw rows
  for (int i = period[0]; i <= period[1]; ++i) {
    cout << setw(2) << i << " | ";

    int current_line_price = history[i];
    // Keep the same upward rounding here
    size_t cells_in_line =
        static_cast<size_t>((current_line_price + price_step - 1) / price_step);

    int event_price_to_show = -1;
    int event_count_to_show = -1;

    for (size_t j = 1; j <= cells_in_line; ++j) {
      int cell_min = static_cast<int>(j - 1) * price_step;
      int cell_max = static_cast<int>(j) * price_step;
      char symbol = '#';

      for (size_t k = 0; k < events.size(); ++k) {
        if (events[k].time == i && events[k].price > cell_min &&
            events[k].price <= cell_max) {
          symbol = events[k].type;
          event_price_to_show = events[k].price;
          event_count_to_show = events[k].count;
          break;
        }
      }
      cout << symbol << "   ";
    }

    // 3. Alignment
    for (size_t space = 0; space < (max_count - cells_in_line); ++space) {
      cout << "    ";
    }

    cout << " (" << setw(2) << current_line_price << ")";
    if (event_price_to_show != -1 && event_count_to_show != -1) {
      cout << " [" << event_price_to_show << " < " << event_count_to_show
           << "]";
    }
    cout << endl;
  }

  // 4. Scale (X axis)
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
    cout << title << "\nNo data for graph.\n";
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
  map<string, vector<Event>> events;

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
    cout << "\nPortfolio:\n";
    cout << "Balance: " << money << "\n";

    bool has_assets = false;
    for (map<Currency, Balance>::iterator item = portfolio.begin();
         item != portfolio.end(); ++item) {
      if (item->second.count <= 0) {
        continue;
      }

      has_assets = true;
      cout << "- " << item->first.get_name() << ": " << item->second.count
           << " units at current price " << item->second.price << "\n";
    }

    if (!has_assets) {
      cout << "No purchased currencies yet.\n";
    }
  }

  void show_total_assets_changes() const {
    cout << "\nPortfolio value changes:\n";

    for (size_t i = 0; i < total_assets_history.size(); ++i) {
      int current_value = total_assets_history[i];
      int delta = 0;

      if (i > 0) {
        delta = current_value - total_assets_history[i - 1];
      }

      cout << "Turn " << i << ": " << current_value;
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
      cout << "Amount must be greater than zero.\n";
      return;
    }

    int total_price = currency.get_price() * count;
    if (total_price > money) {
      cout << "Not enough money to buy.\n";
      return;
    }

    Balance &balance = portfolio[currency];
    balance.price = currency.get_price();
    balance.count += count;
    money -= total_price;

    cout << "Bought " << count << " units of " << currency.get_name() << " for "
         << total_price << ".\n";
    events[currency.get_name()].push_back(
        Event{static_cast<int>(currency.get_history().size() - 1),
              currency.get_price(), count, 'B'});
  }

  void sell(Currency &currency, int count) {
    if (count <= 0) {
      cout << "Amount must be greater than zero.\n";
      return;
    }
    map<Currency, Balance>::iterator item = portfolio.find(currency);
    if (item == portfolio.end() || item->second.count < count) {
      cout << "Not enough currency to sell.\n";
      return;
    }

    int total_price = currency.get_price() * count;
    item->second.count -= count;
    item->second.price = currency.get_price();
    money += total_price;

    cout << "Sold " << count << " units of " << currency.get_name() << " for "
         << total_price << ".\n";
    events[currency.get_name()].push_back(
        Event{static_cast<int>(currency.get_history().size() - 1),
              currency.get_price(), count, 'S'});
  }

  int get_total_assets() const {
    int total_assets = money;

    for (map<Currency, Balance>::const_iterator item = portfolio.begin();
         item != portfolio.end(); ++item) {
      total_assets += item->second.price * item->second.count;
    }

    return total_assets;
  }
  const vector<Event> &get_events(const string &currency_name) {
    return events[currency_name];
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
    cout << "\nAvailable currencies:\n";
    cout << left << setw(4) << "ID" << setw(12) << "Name" << setw(10) << "Price"
         << setw(10) << "Step" << "Vol.\n";

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
        cout << "Currency removed from selected list.\n";
        return;
      }
    }

    selected_currency_indexes.push_back(currency_index);
    cout << "Currency added to selected list.\n";
  }

  void manage_selected_currencies() {
    cout << "\nSelected currency list setup:\n";
    show_currencies(currencies);

    if (selected_currency_indexes.empty()) {
      cout << "The list is currently empty.\n";
    } else {
      cout << "Currently selected:\n";
      for (size_t i = 0; i < selected_currency_indexes.size(); ++i) {
        cout << "- "
             << currencies[static_cast<size_t>(selected_currency_indexes[i])]
                    .get_name()
             << "\n";
      }
    }

    cout << "Enter a currency ID to add or remove.\n";
    cout << "Enter 0 to return to the menu.\n";

    while (true) {
      int currency_id = read_int("Currency ID: ");
      if (currency_id == 0) {
        break;
      }

      if (currency_id < 1 ||
          currency_id > static_cast<int>(currencies.size())) {
        cout << "Currency with this ID does not exist.\n";
        continue;
      }

      toggle_selected_currency(currency_id - 1);
    }
  }

  void show_selected_currencies(Broker &broker) {
    cout << "\nSelected currency list:\n";
    if (selected_currency_indexes.empty()) {
      cout << "The list is empty. Add currencies first.\n";
      return;
    }

    for (size_t i = 0; i < selected_currency_indexes.size(); ++i) {
      Currency &currency =
          currencies[static_cast<size_t>(selected_currency_indexes[i])];

      cout << "\n" << currency.get_name() << "\n";
      cout << "Current price: " << currency.get_price() << "\n";

      show_graph_from_history("Mini graph:", currency.get_history(),
                              currency.get_price_step(),
                              broker.get_events(currency.get_name()));
    }
  }

  void show_portfolio_screen(Broker &broker) {
    broker.show_portfolio();
    broker.show_total_assets_changes();
    show_graph_from_history(
        "Portfolio value mini graph:", broker.get_total_assets_history(),
        calculate_graph_step(broker.get_total_assets_history()));
  }

  void run() {
    map<Currency, Balance> start_portfolio;
    Broker broker(start_portfolio);
    const int max_turns = 25;

    cout << "Welcome to the broker game!\n";
    cout << "Starting capital: 1000\n";
    cout << "After each turn, all currency prices change.\n";

    int turn = 1;
    bool is_game_ended = false;
    while (turn <= max_turns && !is_game_ended) {
      bool is_turn_completed = false;
      while (!is_turn_completed && !is_game_ended) {
        cout << "\n==============================\n";
        cout << "Turn " << turn << " of " << max_turns << "\n";

        broker.refresh_prices(currencies);
        show_currencies(currencies);

        cout << "\nActions:\n";
        cout << "1. Buy currency\n";
        cout << "2. Sell currency\n";
        cout << "3. View portfolio\n";
        cout << "4. Configure selected currency list\n";
        cout << "5. View selected currencies\n";
        cout << "6. End turn\n";
        cout << "0. Exit game\n";

        int action = read_int("Choose an action: ");
        if (action == 0) {
          is_game_ended = true;
          cout << "Game ended early.\n";
          break;
        }

        if (action == 1 || action == 2) {
          int currency_id = read_int("Enter currency ID: ");
          if (currency_id < 1 ||
              currency_id > static_cast<int>(currencies.size())) {
            cout << "Currency with this ID does not exist.\n";
          } else {
            int count = read_int("Enter amount: ");
            Currency &selected_currency =
                currencies[static_cast<size_t>(currency_id - 1)];

            if (action == 1) {
              broker.buy(selected_currency, count);
            } else {
              broker.sell(selected_currency, count);
            }
          }
        } else if (action == 3) {
          show_portfolio_screen(broker);
        } else if (action == 4) {
          manage_selected_currencies();
        } else if (action == 5) {
          show_selected_currencies(broker);
        } else if (action == 6) {
          cout << "Turn ended.\n";
          is_turn_completed = true;
        } else {
          cout << "Unknown action.\n";
        }
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
    cout << "Final state:\n";
    show_currencies(currencies);

    for (size_t i = 0; i < currencies.size(); ++i) {

      int period[2] = {0, static_cast<int>(max_turns)};
      cout << "Currency " << currencies[i].get_name() << " graph:\n";
      draw_graph(build_history_map(currencies[i].get_history()),
                 currencies[i].get_price_step(), period, vector<Event>());
    }

    cout << "\n==============================\n";

    show_portfolio_screen(broker);
    cout << "Total asset value: " << broker.get_total_assets() << "\n";
  }
};
int stop;

int main() {
  vector<Currency> currencies;

  setlocale(LC_ALL, "ru");

  currencies.push_back(Currency("USD", 50, 5, 2));
  currencies.push_back(Currency("EUR", 60, 5, 3));
  currencies.push_back(Currency("BTC", 120, 10, 4));
  currencies.push_back(Currency("ETH", 90, 10, 3));
  currencies.push_back(Currency("XRP", 75, 15, 2));
  currencies.push_back(Currency("SOL", 80, 1, 10));

  Terminal terminal(currencies);
  terminal.run();
  cin >> stop;
}
