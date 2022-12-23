#include <algorithm>
#include <unordered_map>
#include <vector>

#include "util.h"

struct Point {
  int32_t x;
  int32_t y;
  auto operator<=>(const Point &) const = default;
};

auto PointHasher = [](const Point &p) { return (static_cast<int64_t>(p.x) << 32) + p.y; };

auto parseSegmentPoints(const std::string &line) {
  std::istringstream is(line);
  std::vector<Point> points;
  for (std::string tmp; !is.eof(); is >> tmp) {
    char c;
    Point p;
    is >> p.x >> c >> p.y;
    points.push_back(p);
  }
  return points;
}

struct Grid {
  std::unordered_map<Point, char, decltype(PointHasher)> data;
  int32_t ymax{};
  bool has_floor;

  Grid(std::ifstream is, bool has_floor) : has_floor{has_floor} {
    for (std::string line; std::getline(is, line);) {
      auto points = parseSegmentPoints(line);
      for (auto &p : points) ymax = std::max(ymax, p.y);

      for (int i = 1; i < points.size(); i++) {
        auto [from, to] = std::minmax(points[i - 1], points[i]);
        if (from.x == to.x) {
          for (int j = from.y; j <= to.y; j++) data.try_emplace({from.x, j}, '#');
        } else {
          for (int j = from.x; j <= to.x; j++) data.try_emplace({j, from.y}, '#');
        }
      }
    }
    ymax += 2;
  }

  Point getIntersection(Point start) const {
    for (; start.y < ymax; start.y++) {
      if (data.contains(start)) break;
    }
    return start;
  }

  bool isBlocked(const Point &p) const { return p.y == ymax ? has_floor : data.contains(p); }
  bool isAbyss(const Point &p) const { return p.y == ymax && !has_floor; }
  void putSand(const Point &p) { data[p] = '*'; }
};

int getSandSteps(Grid &grid) {
  Point sand_start = grid.getIntersection({500, 0});
  sand_start.y -= 1;
  int steps = 0;

  auto start = sand_start;
  while (!grid.isBlocked(sand_start)) {
    auto hit = grid.getIntersection(start);
    if (grid.isAbyss(hit)) break;
    if (grid.isBlocked({hit.x - 1, hit.y})) {    // Try left.
      if (grid.isBlocked({hit.x + 1, hit.y})) {  // Try right.
        // Settle.
        grid.putSand({hit.x, hit.y - 1});
        if (hit.x == sand_start.x)
          sand_start.y = hit.y > 1 ? std::min(sand_start.y, hit.y - 2) : 0;  // Don't start at the top each time.
      } else {
        start = {hit.x + 1, hit.y};
        continue;
      }
    } else {
      start = {hit.x - 1, hit.y};
      continue;
    }

    start = sand_start;
    steps++;
  }
  return steps;
}

int main() {
  Grid grid1(getInputStream("14.txt"), false);
  Grid grid2(getInputStream("14.txt"), true);

  outputAndVerify("Part 1", getSandSteps(grid1), 625);
  outputAndVerify("Part 2", getSandSteps(grid2), 25193);

  return 0;
}
