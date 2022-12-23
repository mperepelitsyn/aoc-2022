#include <numeric>
#include <unordered_set>
#include <vector>

#include "util.h"

struct Point {
  int16_t x;
  int16_t y;
  int16_t z;

  Point operator+(const Point &other) const {
    return {static_cast<int16_t>(x + other.x), static_cast<int16_t>(y + other.y), static_cast<int16_t>(z + other.z)};
  }
  auto operator<=>(const Point &other) const = default;
  struct Hash {
    size_t operator()(const Point &p) const {
      return (static_cast<uint64_t>(p.x) << 32) + static_cast<uint64_t>(p.y << 16) + (p.z);
    }
  };
};

struct BBox {
  Point min;
  Point max;
};

std::istream &operator>>(std::istream &is, Point &p) {
  char c;
  return is >> p.x >> c >> p.y >> c >> p.z;
}

auto parseInput(std::istream &is) {
  std::vector<Point> out;
  for (Point p{}; is >> p;) out.push_back(p);
  return out;
}

auto getBB(const std::vector<Point> &points) {
  int16_t x_min{INT16_MAX}, x_max{INT16_MIN}, y_min{INT16_MAX}, y_max{INT16_MIN}, z_min{INT16_MAX}, z_max{INT16_MIN};
  for (auto &p : points) {
    x_min = std::min(x_min, p.x);
    y_min = std::min(y_min, p.y);
    z_min = std::min(z_min, p.z);
    x_max = std::max(x_max, p.x);
    y_max = std::max(y_max, p.y);
    z_max = std::max(z_max, p.z);
  }
  return BBox{{y_min, z_min}, {x_max, y_max, z_max}};
}

bool isOutsideBB(const Point &p, const BBox &bb) {
  return p.x < bb.min.x || p.x > bb.max.x || p.y < bb.min.y || p.y > bb.max.y || p.z < bb.min.z || p.z > bb.max.z;
}

void fillOutsideAir(const Point &p, const std::unordered_set<Point, Point::Hash> &droplets, const BBox &bb,
                    std::unordered_set<Point, Point::Hash> &air) {
  air.emplace(p);

  for (auto &dir : {Point{-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1}}) {
    auto newp = p + dir;
    if (isOutsideBB(newp, bb) || droplets.contains(newp) || air.contains(newp)) continue;
    fillOutsideAir(newp, droplets, bb, air);
  }
}

int doPart1(const std::vector<Point> &input) {
  std::unordered_set<Point, Point::Hash> droplets;
  for (auto &in : input) droplets.emplace(in);

  auto area = std::accumulate(droplets.begin(), droplets.end(), 0, [&](auto sum, auto &p) {
    sum += 6;
    for (auto &dir : {Point{-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1}})
      sum -= droplets.contains(p + dir);
    return sum;
  });

  return area;
}

int doPart2(const std::vector<Point> &input) {
  std::unordered_set<Point, Point::Hash> droplets, air;
  auto bb = getBB(input);
  bb.min = bb.min + Point{-1, -1, -1};
  bb.max = bb.max + Point{1, 1, 1};

  for (auto &p : input) droplets.emplace(p);

  fillOutsideAir(bb.min, droplets, bb, air);

  auto area = std::accumulate(droplets.begin(), droplets.end(), 0, [&](auto sum, const auto &p) {
    for (auto &dir : {Point{-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1}})
      sum += air.contains(p + dir);
    return sum;
  });

  return area;
}

int main() {
  auto ifs = getInputStream("18.txt");
  auto input = parseInput(ifs);

  outputAndVerify("Part 1", doPart1(input), 4288);
  outputAndVerify("Part 2", doPart2(input), 2494);

  return 0;
}
