#include <algorithm>
#include <format>
#include <ranges>
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

auto doPart1(const Cave &cave, int t) {
  auto start = cave.valve_to_idx.at("AA");
  auto seen = std::vector(cave.valves.size(), std::vector(t + 1, std::vector(1u << cave.valid_valves, -1)));
  return dp(cave, start, t, (1u << cave.valid_valves) - 1, seen);
}

int main() {
  auto ifs = getInputStream("16.txt");
  auto input = parseInput(ifs);

  outputAndVerify("Part 1: ", doPart1(input, 30), 1796);

  return 0;
}
