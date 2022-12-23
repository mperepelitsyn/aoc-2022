#include <functional>
#include <queue>
#include <vector>

#include "util.h"

struct Grid {
  std::vector<int8_t> data;
  int cols{};
  int rows{};
  size_t start{};
  size_t end{};

  Grid(std::istream &is) {
    for (std::string row; is >> row;) {
      if (!cols) cols = row.size();
      rows++;
      for (auto c : row) {
        if (c == 'S') {
          start = data.size();
        } else if (c == 'E') {
          end = data.size();
        }
        data.push_back(c);
      }
    }
  }

  auto getAdjacentCells(int idx) const {
    std::vector<int> out;
    Point pos{idx % cols, idx / cols};

    for (auto dir : {Point{-1, 0}, {0, -1}, {1, 0}, {0, 1}}) {
      auto adj = pos + dir;
      if (adj.x < 0 || adj.x >= cols || adj.y < 0 || adj.y >= rows) continue;
      out.push_back(adj.y * cols + adj.x);
    }

    return out;
  }

  struct Point {
    int x;
    int y;

    Point operator+(const Point &rhs) const { return {x + rhs.x, y + rhs.y}; }
  };
};

int findShortestPathLen(const Grid &grid, int start_cell, char end_char, auto route_pred) {
  std::queue<int> next;
  next.push(start_cell);
  std::vector<int> prev(grid.data.size(), -1);
  prev[start_cell] = start_cell;
  int end_cell = start_cell;

  while (!next.empty()) {
    auto cell = next.front();
    next.pop();
    for (auto adj : grid.getAdjacentCells(cell)) {
      if (!route_pred(grid.data[adj], grid.data[cell]) || prev[adj] != -1) continue;

      prev[adj] = cell;
      next.push(adj);
      if (grid.data[adj] == end_char) {
        end_cell = adj;
        goto done;
      }
    }
  }

done:
  int len = 0;
  for (auto cell = end_cell; cell != start_cell; cell = prev[cell])
    len++;

  return len;
}

int main() {
  auto fin = getInputStream("12.txt");
  Grid grid{fin};

  auto len_p1 = findShortestPathLen(grid, grid.start, 'E', [](char adj, char cur) {
    return (adj == 'E' ? 'z' : adj) - (cur == 'S' ? 'a' : cur) <= 1;
  });

  auto len_p2 = findShortestPathLen(grid, grid.end, 'a', [](char adj, char cur) {
    return (adj == 'S' ? 'a' : adj) - (cur == 'E' ? 'z' : cur) >= -1;
  });

  outputAndVerify("Part 1", len_p1, 534);
  outputAndVerify("Part 2", len_p2, 525);

  return 0;
}
