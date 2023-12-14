#include <array>
#include <format>
#include <queue>
#include <unordered_set>
#include <vector>

#include "util.h"

enum Kind { ORE, CLAY, OBS, GEODE };
using Material = std::array<int16_t, 4>;
using Blueprint = std::array<Material, 4>;
using Input = std::vector<Blueprint>;

struct State {
  Material robots;
  Material mats;
  int time;
  constexpr std::strong_ordering operator<=>(const State &) const = default;
};
MAKE_HASHABLE(State, *(int64_t *)&t.robots[0], *(int64_t *)&t.mats[0], t.time);

#define BINOP(op)                                          \
  auto operator op(const Material &a, const Material &b) { \
    Material ret;                                          \
    for (auto i = 0; i < 4; ++i) ret[i] = a[i] op b[i];    \
    return ret;                                            \
  }

#define UNOP(op)                                     \
  auto operator op(Material &a, const Material &b) { \
    for (auto i = 0; i < 4; ++i) a[i] op b[i];       \
  }

auto operator*(const Material &a, int b) {
  Material ret;
  for (auto i = 0; i < 4; ++i) ret[i] = a[i] * b;
  return ret;
}

BINOP(-);
BINOP(+);
UNOP(+=);
UNOP(-=);

auto parseInput(std::istream &is) {
  Input input;
  for (std::string w; is >> w >> w;) {
    Blueprint b{};
    is >> w >> w >> w >> w >> b[ORE][ORE] >> w >> w >> w >> w >> w >> b[CLAY][ORE] >> w >> w >> w >> w >> w >>
        b[OBS][ORE] >> w >> w >> b[OBS][CLAY] >> w >> w >> w >> w >> w >> b[GEODE][ORE] >> w >> w >> b[GEODE][OBS] >> w;
    input.push_back(b);
  }
  return input;
}

bool hasEnoughMat(const Blueprint &bp, const Material &mat, int robot) {
  for (int i = 0; i < 4; i++)
    if (mat[i] < bp[robot][i]) return false;
  return true;
}

bool canBuild(const Blueprint &bp, const State &state, const Material &target, int robot) {
  if (state.robots[robot] >= target[robot]) return false;

  for (auto i = 0; i < 3; ++i) {
    if (bp[robot][i] > 0 && state.robots[i] == 0) return false;
  }
  return true;
}

int lowerBound(const Blueprint &bp, State state) {
  while (state.time--) {
    if (hasEnoughMat(bp, state.mats, GEODE)) {
      state.mats += state.robots - bp[GEODE];
      state.robots[GEODE] += 1;
    } else {
      state.mats += state.robots;
    }
  }
  return state.mats[GEODE];
}

int upperBound(const Blueprint &bp, State state) {
  while (state.time--) {
    Material built{};
    for (auto i = 0; i < 4; ++i) {
      if (hasEnoughMat(bp, state.mats, i)) built[i] = 1;
    }

    state.mats += state.robots;
    for (auto i = 0; i < 4; ++i)
      if (built[i]) state.mats - bp[i];
    state.robots += built;
  }
  return state.mats[GEODE];
}

std::optional<State> build(const Blueprint &bp, State state, int robot) {
  int wait = 1;
  for (auto i = 0; i < 3; ++i) {
    auto need = bp[robot][i];
    auto have = state.mats[i];
    if (have >= need) continue;
    wait = std::max(wait, (need - have) / state.robots[i] + ((need - have) % state.robots[i] != 0) + 1);
  }
  if (state.time <= wait) return {};

  state.mats += state.robots * wait - bp[robot];
  state.robots[robot] += 1;
  state.time -= wait;

  return state;
}

auto getMaxRobots(const Blueprint &bp) {
  Material ret{};
  for (auto mat : bp) {
    for (auto i = 0; i < 4; ++i) {
      ret[i] = std::max(ret[i], mat[i]);
    }
  }
  ret[GEODE] = INT16_MAX;
  return ret;
}

int search(const Blueprint &bp, int t) {
  struct PQEl {
    int prio;
    State state;
  };
  auto cmp = [](const PQEl &a, PQEl &b) { return a.prio < b.prio; };
  std::priority_queue<PQEl, std::vector<PQEl>, decltype(cmp)> pq(cmp);
  std::unordered_set<State> seen;
  auto target = getMaxRobots(bp);

  pq.push({1, {Material{1, 0, 0, 0}, {}, t}});
  int best = 0;

  while (!pq.empty()) {
    auto [upper, state] = pq.top();
    pq.pop();

    if (upper <= best) break;

    for (int i = 0; i < 4; ++i) {
      if (state.time < 2 || !canBuild(bp, state, target, i)) continue;

      auto opt = build(bp, state, i);
      if (!opt.has_value()) continue;

      auto next = opt.value();
      auto next_upper = upperBound(bp, next);
      if (next_upper <= best || seen.contains(next)) continue;

      auto next_lower = lowerBound(bp, next);
      best = std::max(best, next_lower);
      pq.push({next_upper, next});
      seen.insert(next);
    }
  }
  return best;
}

int doPart1(const Input &blueprints) {
  int ret = 0;
  for (auto i = 1; auto &bp : blueprints) {
    auto max_geodes = search(bp, 24);
    ret += i++ * max_geodes;
  }
  return ret;
}

int doPart2(const Input &blueprints) {
  int ret = 1;
  for (auto i = 0; i < std::min(blueprints.size(), (size_t)3); ++i) {
    auto max_geodes = search(blueprints[i], 32);
    ret *= max_geodes;
  }
  return ret;
}

int main() {
  auto ifs = getInputStream("19.txt");
  auto blueprints = parseInput(ifs);

  outputAndVerify("Part 1: ", doPart1(blueprints), 1981);
  outputAndVerify("Part 2: ", doPart2(blueprints), 10962);
}
