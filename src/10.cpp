#include <array>

#include "util.h"

int main() {
  auto fin = getInputStream("10.txt");
  std::string cmd;
  int arg;
  int cycle = 0;
  int reg = 1;
  int signal = 0;
  std::array<std::string, 6> crt;

  while (fin >> cmd) {
    int instr_cost = 0;
    if (cmd == "addx") {
      fin >> arg;
      instr_cost = 2;
    } else {
      instr_cost = 1;  // noop.
    }

    while (instr_cost--) {
      if (((cycle + 21) % 40) == 0) signal += (cycle + 1) * reg;

      crt[cycle / 40].push_back(std::abs(reg - cycle % 40) <= 1 ? '#' : '.');
      cycle++;
    }

    if (cmd == "addx") reg += arg;
  }

  outputAndVerify("Part 1", signal, 17940);
  outputAndVerify("Part 2", crt, {"####..##..###...##....##.####...##.####.",
                                  "...#.#..#.#..#.#..#....#.#.......#....#.",
                                  "..#..#....###..#..#....#.###.....#...#..",
                                  ".#...#....#..#.####....#.#.......#..#...",
                                  "#....#..#.#..#.#..#.#..#.#....#..#.#....",
                                  "####..##..###..#..#..##..#.....##..####."});

  return 0;
}
