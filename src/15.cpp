#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <vector>

#include "util.h"

struct Point {
  int32_t x;
  int32_t y;
};

struct Input {
  Point sensor;
  Point beacon;
  long radius;
};

struct Coverage {
  std::vector<std::pair<int, int>> ranges;
  std::unordered_map<int, char> equip;
};

auto dist(const Point &p1, const Point &p2) { return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y); }

auto parseInput(std::istream &is) {
  std::vector<Input> out;
  std::string w;
  char c;
  int x1, x2, y1, y2;
  while (is >> w >> w >> c >> c >> x1 >> c >> c >> c >> y1 >> c >> w >> w >> w >> w >> c >> c >> x2 >> c >> c >> c >>
         y2) {
    out.push_back({{x1, y1}, {x2, y2}, dist({x1, y1}, {x2, y2})});
  }
  return out;
}

auto getRowCoverage(const std::vector<Input> &input, int y) {
  Coverage cov;
  for (auto &in : input) {
    auto ydiff = std::abs(in.sensor.y - y);
    if (ydiff > in.radius) continue;

    auto x1 = in.sensor.x - in.radius + ydiff;
    auto x2 = in.sensor.x + in.radius - ydiff;
    cov.ranges.emplace_back(x1, x2);

    if (in.sensor.y == y) cov.equip.try_emplace(in.sensor.x, 'S');
    if (in.beacon.y == y) cov.equip.try_emplace(in.beacon.x, 'B');
  }
  if (cov.ranges.size() == 1) return cov;

  // Merge.
  std::ranges::sort(cov.ranges, {}, &std::pair<int, int>::first);
  int pos = 0;
  for (int i = 1; i < cov.ranges.size(); i++) {
    if (cov.ranges[pos].second >= cov.ranges[i].first) {
      cov.ranges[pos].second = std::max(cov.ranges[pos].second, cov.ranges[i].second);
    } else {
      cov.ranges[++pos] = cov.ranges[i];
    }
  }
  cov.ranges.erase(cov.ranges.begin() + pos + 1, cov.ranges.end());

  return cov;
}

auto doPart1(const std::vector<Input> &input, int y) {
  auto cov = getRowCoverage(input, y);
  auto covered = std::accumulate(cov.ranges.begin(), cov.ranges.end(), 0,
                                 [](auto sum, auto &r) { return sum + r.second - r.first + 1; });
  return covered - cov.equip.size();
}

auto doPart2(const std::vector<Input> &input, int lim) {
  for (int y = 0; y <= lim; y++) {
    auto cov = getRowCoverage(input, y);
    for (auto &r : cov.ranges) {
      if (r.second >= 0 && r.second <= lim) return (r.second + 1) * 4000000l + y;
    }
  }
  return -1l;
}

int main() {
  auto fis = getInputStream("15.txt");
  auto input = parseInput(fis);

  outputAndVerify("Part 1", doPart1(input, 2000000), 5100463);
  outputAndVerify("Part 2", doPart2(input, 4000000), 11557863040754);

  return 0;
}
