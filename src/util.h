#pragma once

#include <fstream>
#include <iostream>
#include <filesystem>

inline std::ifstream getInputStream(const char *fname) {
#ifdef INPUT_PATH
    std::filesystem::current_path(INPUT_PATH);
#endif
    return std::ifstream{fname};
}

template<class T, size_t N>
std::ostream& operator <<(std::ostream &os, const std::array<T, N> &arr) {
    for (auto &e : arr)
        os << e << std::endl;
    return os;
}

template<class T, class U>
inline void outputAndVerify(const char *desc, T actual, U expected) {
    if (actual != expected) {
        std::cerr << "FAIL: " << desc << ": " << "expected " << expected << ", actual " << actual << std::endl;
        std::exit(1);
    } else {
        std::cout << desc << ": " << actual << std::endl;
    }
}

template<class T, size_t N>
inline void outputAndVerify(const char *desc, const std::array<T, N> &actual, const std::array<T, N> &expected) {
    if (actual != expected) {
        std::cerr << "FAIL: " << desc << ": " << "expected:" << std::endl << expected << "actual:" << std::endl << actual;
        std::exit(1);
    } else {
        std::cout << desc << ":" << std::endl << actual;
    }
}
