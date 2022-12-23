#include <array>
#include <vector>

#include "util.h"

class Cave {
 public:
  Cave(std::string jets) : jets_{std::move(jets)} { reset(); }

  int64_t getHeight(int64_t shapes) {
    auto init_shapes = jets_.size() * shapes_.size();
    if (shapes < 1000000 || shapes <= init_shapes) {
      placeShapes(shapes);
      return height_;
    }
    auto [cycle_shapes, cycle_height] = getCycle(init_shapes);
    auto rem_shapes = shapes - init_shapes;
    placeShapes(init_shapes + rem_shapes % cycle_shapes);

    return height_ + rem_shapes / cycle_shapes * cycle_height;
  }

  void reset() {
    grid_.clear();
    grid_.emplace_back(bottom);
    height_ = shape_pos_ = jet_pos_ = 0;
  }

 private:
  struct Point {
    int64_t x;
    int64_t y;
  };
  struct Shape {
    std::vector<Point> points;
    Point size;
  };
  static const std::array<Shape, 5> shapes_;
  constexpr static const auto empty_row{"|.......|"};
  constexpr static const auto bottom{"+-------+"};

  void placeShapes(int64_t shapes) {
    while (shapes--) {
      placeShape();
    }
  }

  std::pair<int, int> getCycle(int start) {
    int len = shapes_.size();
    placeShapes(start);
    auto copy = *this;

    while (true) {
      *this = copy;
      auto last_height = height_;
      int last_diff{};
      for (int i = 0; i < shapes_.size() * shapes_.size(); i++) {
        placeShapes(len);
        if (last_diff && (height_ - last_height) != last_diff) goto new_len;
        last_diff = height_ - last_height;
        last_height = height_;
      }
      reset();
      return std::make_pair(len, last_diff);

    new_len:
      len++;
    }
  }

  auto hasCollision(const Shape &shape, const Point &pos) const {
    for (auto &p : shape.points) {
      if (grid_[pos.y + p.y][pos.x + p.x] != '.') return true;
    }
    return false;
  }

  void placeShape() {
    Point pos{2 + 1, height_ + 3 + 1};
    auto &shape = shapes_[shape_pos_++ % shapes_.size()];
    if (height_ + shape.size.y + 3 > grid_.size() - 1)
      grid_.insert(grid_.end(), height_ + shape.size.y + 3 - grid_.size() + 1, empty_row);

    while (true) {
      auto jet = jets_[jet_pos_++ % jets_.size()];
      auto xdir = jet == '<' ? -1 : 1;
      if (!hasCollision(shape, {pos.x + xdir, pos.y})) pos.x += xdir;
      if (!hasCollision(shape, {pos.x, pos.y - 1}))
        pos.y--;
      else
        break;
    }
    for (auto &p : shape.points) grid_[pos.y + p.y][pos.x + p.x] = '#';

    height_ = std::max(height_, pos.y - 1 + shape.size.y);
  }

  std::vector<std::string> grid_;
  std::string jets_;
  size_t jet_pos_{};
  size_t shape_pos_{};
  int64_t height_{};
};

const std::array<Cave::Shape, 5> Cave::shapes_{Shape{{{0, 0}, {1, 0}, {2, 0}, {3, 0}}, {4, 1}},
                                               {{{1, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2}}, {3, 3}},
                                               {{{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}}, {3, 3}},
                                               {{{0, 0}, {0, 1}, {0, 2}, {0, 3}}, {1, 4}},
                                               {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {2, 2}}};

int main() {
  auto ifs = getInputStream("17.txt");
  std::string jets;
  ifs >> jets;
  Cave cave{jets};

  outputAndVerify("Part 1", cave.getHeight(2022), 3137);
  cave.reset();
  outputAndVerify("Part 2", cave.getHeight(1000000000000), 1564705882327);

  return 0;
}
