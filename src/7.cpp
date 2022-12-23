#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>

#include "util.h"

class FS {
 public:
  struct Node {
    Node(Node *parent, size_t size) : parent{parent}, size{size} {}

    std::unordered_map<std::string, std::unique_ptr<Node>> children;
    Node *parent;
    size_t size;  // 0 for folders;
  };
  using walkCb = std::function<void(const std::string &, size_t)>;

  void tryAddNode(const std::string &name, size_t size) {
    cwd_->children.try_emplace(name, std::make_unique<Node>(cwd_, size));
  }

  void changeWorkDir(const std::string &dir) {
    if (dir == "/")
      cwd_ = root_.get();
    else if (dir == "..")
      cwd_ = cwd_->parent;
    else
      cwd_ = cwd_->children.at(dir).get();
  }

  void walkForders(walkCb cb) { DFS("/", root_.get(), cb); }

 private:
  size_t DFS(const std::string &name, const Node *node, walkCb cb) {
    if (node->size != 0) return node->size;

    size_t size = 0;
    for (auto &p : node->children) size += DFS(p.first, p.second.get(), cb);

    cb(name, size);

    return size;
  }

  std::unique_ptr<Node> root_{std::make_unique<Node>(nullptr, 0)};
  Node *cwd_;
};

int main() {
  auto fin = getInputStream("7.txt");
  FS fs;

  for (std::string tmp, cmd, arg; fin >> tmp >> cmd;) {
    if (cmd == "cd") {
      fin >> arg;
      fs.changeWorkDir(arg);
    } else if (cmd == "ls") {
      std::string size_or_dir, name;
      char cmd_sign;
      while (fin >> std::skipws >> size_or_dir >> name) {
        fs.tryAddNode(name, size_or_dir == "dir" ? 0 : std::stoull(size_or_dir));
        fin >> cmd_sign;
        fin.putback(cmd_sign);
        if (cmd_sign == '$') break;
      }
    } else {
      abort();
    }
  }

  size_t p1_total = 0;
  size_t used = 0;
  std::vector<size_t> sizes;
  fs.walkForders([&](const std::string &name, size_t size) {
    if (name == "/") {
      used = size;
    } else {
      if (size <= 100000) p1_total += size;
      sizes.push_back(size);
    }
  });

  size_t freeup = 30000000 - (70000000 - used);
  std::ranges::sort(sizes);
  auto it = std::ranges::find_if(sizes, [=](auto size) { return size >= freeup; });

  outputAndVerify("Part 1", p1_total, 1749646);
  outputAndVerify("Part 2", *it, 1498966);

  return 0;
}
