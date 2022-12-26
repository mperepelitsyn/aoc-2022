#include <algorithm>
#include <array>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "util.h"

struct Point {
  int32_t x;
  int32_t y;

  struct Hash {
    size_t operator()(const Point &p) const { return (static_cast<int64_t>(p.x) << 32) + p.y; }
  };

  auto operator+=(const Point &other) {
    x += other.x;
    y += other.y;
  }
  Point operator+(const Point &other) const { return {x + other.x, y + other.y}; }
  Point operator-(const Point &other) const { return {x - other.x, y - other.y}; }
  bool operator==(const Point &) const = default;
};

struct Elf {
  Point pos;
  Point next_pos;
};

struct AABB {
  Point min;
  Point max;
};

using Input = std::vector<std::unique_ptr<Elf>>;
using Grid = std::unordered_set<Point, Point::Hash>;
using PlannedGrid = std::unordered_map<Point, Elf *, Point::Hash>;

constexpr Point N{0, -1}, S{0, 1}, W{-1, 0}, E{1, 0};

auto parseInput(std::istream &is) {
  Input in;
  int row{};
  for (std::string line; is >> line; row++) {
    for (int col{}; col < line.size(); col++) {
      if (line[col] == '#') in.emplace_back(std::make_unique<Elf>(Elf{col, row}));
    }
  }
  return in;
}

bool isAlone(const Grid &grid, const Point &p, const Point &dir) {
  if (dir == N || dir == S)
    return !grid.contains(p + dir) && !grid.contains(p + dir + W) && !grid.contains(p + dir + E);
  else
    return !grid.contains(p + dir) && !grid.contains(p + dir + N) && !grid.contains(p + dir + S);
}

bool isAlone(const Grid &grid, const Point &p) {
  for (auto &dir : {N, S, E, W}) {
    if (!isAlone(grid, p, dir)) return false;
  }
  return true;
}

auto getAABB(const Input &elves) {
  auto [x_min, x_max] = std::ranges::minmax(elves | std::views::transform([](auto &e) { return e->pos.x; }));
  auto [y_min, y_max] = std::ranges::minmax(elves | std::views::transform([](auto &e) { return e->pos.y; }));
  return AABB{{x_min, y_min}, {x_max, y_max}};
}

bool spread(Input &elves, size_t &dir_idx, PlannedGrid &plan, Grid &grid) {
  static constexpr std::array<Point, 4> dirs{N, S, W, E};

  for (auto &elf : elves) {
    if (isAlone(grid, elf->pos)) {
      elf->next_pos = elf->pos;
      continue;
    }
    elf->next_pos = elf->pos;

    for (auto i = dir_idx; i < dir_idx + dirs.size(); i++) {
      auto &dir = dirs[i % dirs.size()];
      if (isAlone(grid, elf->pos, dir)) {
        auto it = plan.find(elf->pos + dir);
        if (it != plan.end()) {
          it->second->next_pos = it->second->pos;
        } else {
          elf->next_pos = elf->pos + dir;
          plan.emplace(elf->next_pos, elf.get());
        }
        break;
      }
    }
  }
  plan.clear();
  grid.clear();

  bool moved = false;
  for (auto &elf : elves) {
    if (elf->pos != elf->next_pos) moved = true;
    elf->pos = elf->next_pos;
    grid.emplace(elf->pos);
  }

  dir_idx++;

  return moved;
}

auto doPart1(Input elves, int rounds) {
  size_t dir_idx{};
  PlannedGrid plan;
  Grid grid;
  std::ranges::for_each(elves, [&](auto &e) { grid.emplace(e->pos); });

  while (rounds--) spread(elves, dir_idx, plan, grid);

  auto [bb_min, bb_max] = getAABB(elves);

  return (bb_max.x - bb_min.x + 1) * (bb_max.y - bb_min.y + 1) - elves.size();
}

auto doPart2(Input elves) {
  size_t dir_idx{};
  PlannedGrid plan;
  Grid grid;
  std::ranges::for_each(elves, [&](auto &e) { grid.emplace(e->pos); });

  bool moved = true;
  int rounds{};
  while (moved) {
    moved = spread(elves, dir_idx, plan, grid);
    rounds++;
  }

  return rounds;
}

int main() {
  auto ifs = getInputStream("23.txt");

  outputAndVerify("Part 1", doPart1(parseInput(ifs), 10), 4138);
  ifs.clear();
  ifs.seekg(0);

  outputAndVerify("Part 2", doPart2(parseInput(ifs)), 1010);

  return 0;
}
