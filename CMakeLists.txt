cmake_minimum_required(VERSION 3.24)
project(AoC CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CXX_FLAGS_ASAN "-fsanitize=address,undefined -g -fno-omit-frame-pointer")
add_compile_definitions(INPUT_PATH="${CMAKE_BINARY_DIR}/input")

include(CTest)

if(NOT DEFINED ENV{AOC_SESSION})
    message(FATAL_ERROR "AOC_SESSION token is not set")
endif()

foreach(day RANGE 1 25)
    if (day IN_LIST skip_days)
        continue()
    endif()
    set(url https://adventofcode.com/2022/day/${day}/input)
    message("Downloading ${url}...")
    file(DOWNLOAD ${url} input/${day}.txt HTTPHEADER "Cookie: session=$ENV{AOC_SESSION};")
    add_executable(${day} src/${day}.cpp)
    add_test(NAME "Day ${day}" COMMAND ${day})
endforeach()
