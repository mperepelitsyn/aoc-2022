#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

inline std::ifstream getInputStream(const char *fname) {
#ifdef INPUT_PATH
  std::filesystem::current_path(INPUT_PATH);
#endif
  return std::ifstream{fname};
}

template <class T, size_t N>
std::ostream &operator<<(std::ostream &os, const std::array<T, N> &arr) {
  for (auto &e : arr) os << e << std::endl;
  return os;
}

template <class T, class U>
inline void outputAndVerify(const char *desc, T actual, U expected) {
  if (actual != expected) {
    std::cerr << "FAIL: " << desc << ": "
              << "expected " << expected << ", actual " << actual << std::endl;
    std::exit(1);
  } else {
    std::cout << desc << ": " << actual << std::endl;
  }
}

template <class T, size_t N>
inline void outputAndVerify(const char *desc, const std::array<T, N> &actual, const std::array<T, N> &expected) {
  if (actual != expected) {
    std::cerr << "FAIL: " << desc << ": "
              << "expected:" << std::endl
              << expected << "actual:" << std::endl
              << actual;
    std::exit(1);
  } else {
    std::cout << desc << ":" << std::endl << actual;
  }
}

inline void hash_combine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
  hash_combine(seed, rest...);
}

#define MAKE_HASHABLE(type, ...) \
    namespace std {\
        template<> struct hash<type> {\
            std::size_t operator()(const type &t) const {\
                std::size_t ret = 0;\
                hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }
