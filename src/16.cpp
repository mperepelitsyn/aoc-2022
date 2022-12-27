#include <unordered_map>
#include <vector>
#include "util.h"

struct Valve {
    std::string name;
    std::vector<int> tunnels;
    int pressure{};
};

struct Cave {
    std::vector<Valve> valves;
    std::unordered_map<std::string, int> valve_to_idx;
    uint64_t closed{};
};

#define SETBIT(num, bit) ((num) | (1ull << (bit)))
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
            if (is.peek() == ',') is.ignore();
            else break;
        }
        auto idx = cave.valve_to_idx[v.name];
        if (v.pressure) cave.closed = SETBIT(cave.closed, idx);
        cave.valves[idx] = v;
    }
    return cave;
}

struct Result {
    int press_released;
    uint64_t valves_closed;
    auto operator<=>(const Result &rhs) const { return press_released <=> rhs.press_released; }
};

Result findMaxPress(std::vector<Valve> &valves, int idx, int prev, int step, int press, uint64_t closed) {
    Result max = { press, closed };
    if (step == 0 || closed == 0) return max;

    if (CHECKBIT(closed, idx)) {
        max = std::max(max, findMaxPress(valves, idx, prev, step - 1, press + valves[idx].pressure * (step - 1), CLEARBIT(closed, idx)));
    }
    for (auto tunnel : valves[idx].tunnels) {
        if (valves[idx].tunnels.size() > 1 && tunnel == prev)
            continue;  // Don't go immediately back.
        max = std::max(max, findMaxPress(valves, tunnel, idx, step - 1, press, closed));
    }
    return max;
}

int findMaxPress2(std::vector<Valve> &valves, int idx1, int prev1, int idx2, int prev2, int step, int press, uint64_t closed) {
    if (step == 0 || closed == 0) return press;

    int32_t max = press;
    if (CHECKBIT(closed, idx1)) {
        // Both open.
        if (idx1 != idx2 && CHECKBIT(closed, idx2)) {
            max = std::max(max,
                           findMaxPress2(valves, idx1, prev1, idx2, prev2, step - 1,
                                         press + valves[idx1].pressure * (step - 1) + valves[idx2].pressure * (step - 1), CLEARBIT(CLEARBIT(closed, idx1), idx2)));
        }

        // 1 opens, 2 goes.
        for (auto tunnel : valves[idx2].tunnels) {
            if (valves[idx2].tunnels.size() > 1 && tunnel == prev2)
                continue;  // Don't go immediately back.
            max = std::max(max, findMaxPress2(valves, idx1, prev1, tunnel, idx2, step - 1, press + valves[idx1].pressure * (step - 1), CLEARBIT(closed, idx1)));
        }
    }
    if (idx1 != idx2 && CHECKBIT(closed, idx2)) {
        // 2 opens, 1 goes.
        for (auto tunnel : valves[idx1].tunnels) {
            if (valves[idx1].tunnels.size() > 1 && tunnel == prev1)
                continue;  // Don't go immediately back.
            max = std::max(max, findMaxPress2(valves, tunnel, idx1, idx2, prev2, step - 1, press + valves[idx2].pressure * (step - 1), CLEARBIT(closed, idx2)));
        }
    }

    // Everyone goes.
    for (auto tunnel1 : valves[idx1].tunnels) {
        for (auto tunnel2 : valves[idx2].tunnels) {
            if (valves[idx1].tunnels.size() > 1 && tunnel1 == prev1)
                continue;  // Don't go immediately back.
            if (valves[idx2].tunnels.size() > 1 && tunnel2 == prev2)
                continue;  // Don't go immediately back.
            max = std::max(max, findMaxPress2(valves, tunnel1, idx1, tunnel2, idx2, step - 1, press, closed));
        }
    }
    return max;
}

auto doPart1(Cave &input) {
    auto start_idx = input.valve_to_idx.at("AA");
    return findMaxPress(input.valves, start_idx, 0, 30, 0, input.closed);
}

auto doPart2(Cave &input) {
    auto start_idx = input.valve_to_idx.at("AA");
    auto human = findMaxPress(input.valves, start_idx, 0, 26, 0, input.closed);
    auto elephant = findMaxPress(input.valves, start_idx, 0, 26, 0, human.valves_closed);
    return human.press_released + elephant.press_released;
}

int main() {
    auto ifs = getInputStream("16.txt");
    auto input = parseInput(ifs);

    auto n = doPart1(input).press_released;
    //auto n = doPart2(input);

    std::cout << n;
    return 0;
}
