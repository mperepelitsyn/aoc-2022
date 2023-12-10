#include <unordered_map>
#include <vector>

#include "util.h"

struct Valve {
  std::string name;
  std::vector<int> tunnels;
  int pressure{};
  int id{-1};
};

struct Cave {
  std::vector<Valve> valves;
  std::unordered_map<std::string, int> valve_to_idx;
  int valid_valves{};
};

#define CLEARBIT(num, bit) ((num) & ~(1ull << (bit)))
#define CHECKBIT(num, bit) ((num) & (1ull << (bit)))

auto parseInput(std::istream &is) {
  Cave cave;
  int valve_count{};
  for (std::string w; is >> w >> w;) {
    cave.valve_to_idx[w] = valve_count++;
    is.ignore(1000, '\n');
  }
  cave.valves.insert(cave.valves.end(), valve_count, {});
  is.clear();
  is.seekg(0);

  std::string w;
  char c;
  while (is >> w) {
    Valve v;
    is >> v.name;
    is.ignore(100, '=');
    is >> v.pressure;
    is >> c >> w >> w >> w >> w;
    for (char vname[2]; is >> vname[0] >> vname[1];) {
      v.tunnels.push_back(cave.valve_to_idx.at({vname, 2}));
      if (is.peek() == ',')
        is.ignore();
      else
        break;
    }
    auto idx = cave.valve_to_idx[v.name];
    if (v.pressure) v.id = cave.valid_valves++;
    cave.valves[idx] = v;
  }
  return cave;
}

int dp(const Cave &cave, int node, int time, unsigned closed, std::vector<std::vector<std::vector<int>>> &seen) {
  if (time <= 0 || closed == 0) return 0;
  if (seen[node][time][closed] != -1) return seen[node][time][closed];

  int ret = 0;
  for (auto next : cave.valves[node].tunnels) {
    if (cave.valves[node].pressure && CHECKBIT(closed, cave.valves[node].id))
      ret = std::max(ret, (cave.valves[node].pressure * (time - 1) +
                           dp(cave, next, time - 2, CLEARBIT(closed, cave.valves[node].id), seen)));
    ret = std::max(ret, dp(cave, next, time - 1, closed, seen));
  }

  seen[node][time][closed] = ret;
  return ret;
}

int dp(const Cave &cave, int p1, int p2, int time, unsigned closed,
       std::vector<std::vector<std::vector<std::vector<int>>>> &seen) {
  if (time <= 0 || closed == 0) return 0;
  if (seen[p1][p2][time][closed] != -1) return seen[p1][p2][time][closed];

  auto &p1v = cave.valves[p1];
  auto &p2v = cave.valves[p2];
  auto can_open = [&](int node) { return cave.valves[node].pressure && CHECKBIT(closed, cave.valves[node].id); };

  int ret = 0;
  if (can_open(p1)) {
    for (auto p2n : cave.valves[p2].tunnels) {
      ret = std::max(ret, p1v.pressure * (time - 1) + dp(cave, p1, p2n, time - 1, CLEARBIT(closed, p1v.id), seen));
    }
  } else if (can_open(p2)) {
    for (auto p1n : cave.valves[p1].tunnels) {
      ret = std::max(ret, p2v.pressure * (time - 1) + dp(cave, p1n, p2, time - 1, CLEARBIT(closed, p2v.id), seen));
    }
  }

  for (auto p1n : cave.valves[p1].tunnels) {
    for (auto p2n : cave.valves[p2].tunnels) {
      if (p1 != p2 && can_open(p1) && can_open(p2)) {
        ret = std::max(ret, p1v.pressure * (time - 1) + p2v.pressure * (time - 1) +
                                dp(cave, p1n, p2n, time - 2, CLEARBIT(CLEARBIT(closed, p1v.id), p2v.id), seen));
      }
      ret = std::max(ret, dp(cave, p1n, p2n, time - 1, closed, seen));
    }
  }

  seen[p1][p2][time][closed] = ret;
  return ret;
}

auto doPart1(const Cave &cave) {
  constexpr auto time = 30;
  auto start = cave.valve_to_idx.at("AA");
  auto seen = std::vector(cave.valves.size(), std::vector(time + 1, std::vector(1u << cave.valid_valves, -1)));
  return dp(cave, start, time, (1u << cave.valid_valves) - 1, seen);
}

auto doPart2(const Cave &cave) {
  constexpr auto time = 26;
  auto start = cave.valve_to_idx.at("AA");
  auto seen =
      std::vector(cave.valves.size(),
                  std::vector(cave.valves.size(), std::vector(time + 1, std::vector(1u << cave.valid_valves, -1))));
  return dp(cave, start, start, time, (1u << cave.valid_valves) - 1, seen);
}

int main() {
  auto ifs = getInputStream("16.txt");
  auto input = parseInput(ifs);

  outputAndVerify("Part 1: ", doPart1(input), 1796);
  outputAndVerify("Part 2: ", doPart2(input), 1999);

  return 0;
}
