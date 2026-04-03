#include <iostream>
#include <map>
#include <vector>

using namespace std;

/*
TODO: 123
 * Создать список валют
 * Создать генератор валют
*/

void draw_graph() {}

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

int main() { std::cout << "Hello World!\n"; }
