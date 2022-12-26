#include <algorithm>
#include <vector>

#include "util.h"

struct Point {
  int x;
  int y;

  void rotate(int in) {
    auto tmp = x;
    if (in == -1) {  // L.
      x = y;
      y = -tmp;
    } else {  // R.
      x = -y;
      y = tmp;
    }
  }
  auto operator+=(const Point &other) {
    x += other.x;
    y += other.y;
  }
  Point operator+(const Point &other) const { return {x + other.x, y + other.y}; }
  Point operator-(const Point &other) const { return {x - other.x, y - other.y}; }
  bool operator==(const Point &) const = default;
};

using Grid = std::vector<std::vector<char>>;
using Input = std::pair<Grid, std::vector<int>>;

Input parseInput(std::istream &is) {
  Input in;
  size_t cols{}, rows{};
  for (std::string line; std::getline(is, line);) {
    if (line.empty()) break;
    cols = std::max(cols, line.size());
    rows++;
  }
  cols += 2;
  rows += 2;
  in.first.resize(rows);

  in.first.front().insert(in.first.front().end(), cols, ' ');
  size_t row = 1;
  is.seekg(0);
  for (std::string line; std::getline(is, line);) {
    if (line.empty()) break;
    in.first[row].push_back(' ');
    in.first[row].insert(in.first[row].end(), line.begin(), line.end());
    in.first[row].insert(in.first[row].end(), cols - in.first[row].size(), ' ');
    row++;
  }
  in.first.back().insert(in.first.back().end(), cols, ' ');

  while (true) {
    char dir;
    int moves;
    if (std::isdigit(is.peek())) {
      is >> moves;
      in.second.push_back(moves);
    } else if (std::isalpha(is.peek())) {
      is >> dir;
      in.second.push_back(dir == 'L' ? -1 : -2);
    } else {
      break;
    }
  }
  return in;
}

auto wrapAround(const Grid &grid, Point &dir, Point &pos) {
  auto newdir = dir;
  newdir.rotate(-1);
  newdir.rotate(-1);
  pos += newdir;
  for (; grid[pos.y][pos.x] != ' '; pos += newdir)
    ;
  pos += dir;
}

auto getCubeSideIdx(const Point &p, int cube_size) {
  if (p.y <= cube_size) {
    if (p.x <= 2 * cube_size)
      return 1;
    else
      return 2;
  } else if (p.y <= cube_size * 2) {
    return 3;
  } else if (p.y <= 3 * cube_size) {
    if (p.x <= cube_size)
      return 4;
    else
      return 5;
  } else {
    return 6;
  }
}

auto wrapAroundCube(const Grid &, Point &dir, Point &pos) {
  static constexpr Point up{0, -1}, down{0, 1}, left{-1, 0}, right{1, 0};
  static constexpr auto cube_size = 50;

  pos = pos - dir;
  auto side = getCubeSideIdx(pos, cube_size);
  if (side == 1) {
    if (dir == up) {  // 6.
      pos = {1, pos.x + 2 * cube_size};
      dir = right;
    } else if (dir == left) {  // 4.
      pos = {1, 1 + 3 * cube_size - pos.y};
      dir = right;
    }
  } else if (side == 2) {
    if (dir == up) {  // 6.
      pos = {pos.x - 2 * cube_size, 4 * cube_size};
      dir = up;
    } else if (dir == right) {  // 5.
      pos = {2 * cube_size, 1 + 3 * cube_size - pos.y};
      dir = left;
    } else {  // 3.
      pos = {2 * cube_size, pos.x - cube_size};
      dir = left;
    }
  } else if (side == 3) {
    if (dir == left) {  // 4.
      pos = {pos.y - cube_size, 2 * cube_size + 1};
      dir = down;
    } else {  // 2.
      pos = {pos.y + cube_size, cube_size};
      dir = up;
    }
  } else if (side == 4) {
    if (dir == up) {  // 3.
      pos = {cube_size + 1, pos.x + cube_size};
      dir = right;
    } else {  // 1.
      pos = {cube_size + 1, 1 + 3 * cube_size - pos.y};
      dir = right;
    }
  } else if (side == 5) {
    if (dir == right) {  // 2.
      pos = {3 * cube_size, 1 + 3 * cube_size - pos.y};
      dir = left;
    } else {  // 6.
      pos = {cube_size, 2 * cube_size + pos.x};
      dir = left;
    }
  } else {               // 6.
    if (dir == right) {  // 5.
      pos = {pos.y - 2 * cube_size, 3 * cube_size};
      dir = up;
    } else if (dir == down) {  // 2.
      pos = {pos.x + 2 * cube_size, 1};
      dir = down;
    } else {  // 1.
      pos = {pos.y - 2 * cube_size, 1};
      dir = down;
    }
  }
}

auto mapDirToNum(const Point &dir) {
  if (dir == Point{1, 0}) return 0;
  if (dir == Point{0, 1}) return 1;
  if (dir == Point{-1, 0}) return 2;
  return 3;
}

auto walk(Input in, auto &&wrap_fn) {
  auto &grid = in.first;
  auto it = std::ranges::find(grid[1], '.');
  Point pos{static_cast<int>(it - grid[1].begin()), 1};
  Point dir{1, 0};

  for (auto moves : in.second) {
    if (moves < 0) {
      dir.rotate(moves);
      continue;
    }
    while (moves--) {
      auto newpos = pos + dir;
      auto newdir = dir;
      if (grid[newpos.y][newpos.x] == ' ') {
        wrap_fn(grid, newdir, newpos);
      }
      if (grid[newpos.y][newpos.x] == '#') {
        break;
      }
      pos = newpos;
      dir = newdir;
    }
  }

  return 1000ull * pos.y + 4 * pos.x + mapDirToNum(dir);
}

int main() {
  auto ifs = getInputStream("22.txt");
  auto input = parseInput(ifs);

  outputAndVerify("Part 1", walk(input, wrapAround), 43466);
  outputAndVerify("Part 2", walk(input, wrapAroundCube), 162155);

  return 0;
}
