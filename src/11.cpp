#include <algorithm>
#include <numeric>
#include <vector>

#include "util.h"

struct Monkey {
  enum Op { ADD, MUL, SQUARE };

  std::vector<uint64_t> items;
  Op op;
  int op_arg;
  int test_arg;
  int dst_on_true;
  int dst_on_false;
  uint64_t inspected;
};

auto parseInput(std::istream &is) {
  std::vector<Monkey> monkeys;

  for (std::string w; is >> w >> w >> w >> w;) {
    Monkey monkey{};
    for (int worry_lvl; is >> worry_lvl;) {
      monkey.items.push_back(worry_lvl);
      if (is.peek() == ',')
        is.ignore();
      else
        break;
    }
    char op;
    is >> w >> w >> w >> w >> op >> w;
    if (op == '+') {
      monkey.op = Monkey::ADD;
      monkey.op_arg = std::stoi(w);
    } else {
      if (w == "old") {
        monkey.op = Monkey::SQUARE;
      } else {
        monkey.op = Monkey::MUL;
        monkey.op_arg = std::stoi(w);
      }
    }
    is >> w >> w >> w >> monkey.test_arg;
    is >> w >> w >> w >> w >> w >> monkey.dst_on_true;
    is >> w >> w >> w >> w >> w >> monkey.dst_on_false;

    monkeys.push_back(std::move(monkey));
  }

  return monkeys;
}

auto calcBusiness(std::vector<Monkey> monkeys, int rounds, int relief = 1) {
  int lcm = 1;
  for (auto &monkey : monkeys) lcm = std::lcm(lcm, monkey.test_arg);

  for (int round = 0; round < rounds; round++) {
    for (auto &monkey : monkeys) {
      for (auto item : monkey.items) {
        auto worry_lvl = monkey.op == Monkey::ADD   ? item + monkey.op_arg
                         : monkey.op == Monkey::MUL ? item * monkey.op_arg
                                                    : item * item;
        worry_lvl %= lcm;
        worry_lvl /= relief;

        if ((worry_lvl % monkey.test_arg) == 0)
          monkeys[monkey.dst_on_true].items.push_back(worry_lvl);
        else
          monkeys[monkey.dst_on_false].items.push_back(worry_lvl);

        monkey.inspected++;
      }
      monkey.items.clear();
    }
  }
  std::sort(monkeys.begin(), monkeys.end(), [](auto &a, auto b) { return a.inspected >= b.inspected; });

  return monkeys[0].inspected * monkeys[1].inspected;
}

int main() {
  auto fin = getInputStream("11.txt");
  auto input = parseInput(fin);

  outputAndVerify("Part 1", calcBusiness(input, 20, 3), 57838);
  outputAndVerify("Part 2", calcBusiness(input, 10000, 1), 15050382231);

  return 0;
}
