#include <deque>
#include <unordered_set>
#include <vector>

#include "util.h"

using Pos = std::tuple<int, int>;

class Valley {
 public:
  using Grid = std::vector<std::string>;

  explicit Valley(const Grid &grid) : hor_(grid.size()), vert_(grid[0].size()), m_(grid.size()), n_(grid[0].size()) {
    genWinds(grid);
  }

  auto static getStartPos() { return std::tuple(0, 1); }
  auto getEndPos() const { return std::tuple(m_ - 1, n_ - 2); }
  auto getHorWindTime(int time) const { return time % (n_ - 2); }
  auto getVertWindTime(int time) const { return time % (m_ - 2); }
  auto isWindHit(uint8_t i, uint8_t j, int time) const {
    return isWindHit(hor_[i], j, n_, time) || isWindHit(vert_[j], i, m_, time);
  }
  auto isValidPos(uint8_t i, uint8_t j) const {
    return i == m_ - 1 && j == n_ - 2 || i == 0 && j == 1 || i > 0 && i < m_ - 1 && j > 0 && j < n_ - 1;
  }

 private:
  using Wind = std::vector<std::pair<int8_t, uint8_t>>;
  using Winds = std::vector<Wind>;

  static int getWindCycle(int len) { return (len - 2) * 2 - 2; }

  void genWinds(const Grid &grid) {
    for (uint8_t i = 0; i < grid.size(); ++i) {
      for (uint8_t j = 0; j < grid[0].size(); ++j) {
        if (grid[i][j] == '>')
          hor_[i].emplace_back(1, j);
        else if (grid[i][j] == '<')
          hor_[i].emplace_back(-1, j);
        else if (grid[i][j] == 'v')
          vert_[j].emplace_back(1, i);
        else if (grid[i][j] == '^')
          vert_[j].emplace_back(-1, i);
      }
    }
  }

  static bool isWindHit(const Wind &wind, uint8_t p, uint8_t len, int time) {
    len -= 2;
    time %= len;

    for (auto [dir, pos] : wind) {
      if (p == (len + pos - 1 + dir * time) % len + 1) return true;
    }
    return false;
  }

  Winds hor_;
  Winds vert_;
  uint8_t m_;
  uint8_t n_;
};

auto parseInput(std::istream &is) {
  Valley::Grid ret;
  int row{};
  for (std::string line; is >> line; row++) {
    ret.push_back(std::move(line));
  }
  return ret;
}

struct State {
  uint8_t i;
  uint8_t j;
  uint8_t hor;
  uint8_t vert;
  bool operator==(const State &) const = default;
};

MAKE_HASHABLE(State, t.i, t.j, t.hor, t.vert);

auto search(const Valley &val, const Pos &start, const Pos &end, int time) {
  std::deque<std::tuple<int8_t, int8_t, uint16_t>> q;
  std::unordered_set<State> seen;
  q.emplace_back(std::get<0>(start), std::get<1>(start), time);
  seen.insert({0, 1, 0, 0});

  while (!q.empty()) {
    auto [i, j, t] = q.front();
    q.pop_front();

    for (auto [ni, nj] : {std::pair(i + 1, j), {i - 1, j}, {i, j + 1}, {i, j - 1}, {i, j}}) {
      if (Pos{ni, nj} == end) return t + 1;
      if (!val.isValidPos(ni, nj) || val.isWindHit(ni, nj, t + 1)) continue;
      auto key = State(ni, nj, val.getHorWindTime(t + 1), val.getVertWindTime(t + 1));
      if (seen.contains(key)) continue;

      seen.insert(key);
      q.emplace_back(ni, nj, t + 1);
    }
  }

  return -1;
}

auto doPart1(const Valley &val) { return search(val, val.getStartPos(), val.getEndPos(), 0); }

auto doPart2(const Valley &val) {
  auto t = search(val, val.getStartPos(), val.getEndPos(), 0);
  t = search(val, val.getEndPos(), val.getStartPos(), t);
  return search(val, val.getStartPos(), val.getEndPos(), t);
}

int main() {
  auto ifs = getInputStream("24.txt");
  auto grid = parseInput(ifs);
  Valley valley(grid);

  outputAndVerify("Part 1", doPart1(valley), 249);
  outputAndVerify("Part 2", doPart2(valley), 735);

  return 0;
}
