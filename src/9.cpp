#include <set>
#include <vector>

#include "util.h"

struct Point {
  int x;
  int y;
  auto operator<=>(const Point&) const = default;
};

using Input = std::vector<std::pair<char, int>>;

auto parseInput(std::istream& is) {
  std::vector<std::pair<char, int>> out;
  char dir;
  int steps;
  while (is >> dir >> steps) out.emplace_back(dir, steps);
  return out;
}

auto countTailPos(const Input& input, int rope_count) {
  std::set<Point> set;
  std::vector<Point> ropes(rope_count);
  for (auto [dir, steps] : input) {
    while (steps--) {
      if (dir == 'U')
        ropes[0].y++;
      else if (dir == 'D')
        ropes[0].y--;
      else if (dir == 'L')
        ropes[0].x--;
      else
        ropes[0].x++;

      for (auto i = 1u; i < ropes.size(); i++) {
        if (std::abs(ropes[i - 1].x - ropes[i].x) == 2 && std::abs(ropes[i - 1].y - ropes[i].y) == 2) {
          ropes[i].x = (ropes[i].x + ropes[i - 1].x) / 2;
          ropes[i].y = (ropes[i].y + ropes[i - 1].y) / 2;
        } else if (std::abs(ropes[i - 1].x - ropes[i].x) == 2) {
          if (ropes[i - 1].y == ropes[i].y) {
            ropes[i].x = (ropes[i].x + ropes[i - 1].x) / 2;
          } else {
            ropes[i].x = (ropes[i].x + ropes[i - 1].x) / 2;
            ropes[i].y = ropes[i - 1].y;
          }
        } else if (std::abs(ropes[i - 1].y - ropes[i].y) == 2) {
          if (ropes[i - 1].x == ropes[i].x) {
            ropes[i].y = (ropes[i].y + ropes[i - 1].y) / 2;
          } else {
            ropes[i].x = ropes[i - 1].x;
            ropes[i].y = (ropes[i].y + ropes[i - 1].y) / 2;
          }
        }
      }
      set.insert(ropes.back());
    }
  }
  return set.size();
}

int main() {
  auto fin = getInputStream("9.txt");
  auto input = parseInput(fin);

  outputAndVerify("Part 1", countTailPos(input, 2), 6018);
  outputAndVerify("Part 2", countTailPos(input, 10), 2619);

  return 0;
}
