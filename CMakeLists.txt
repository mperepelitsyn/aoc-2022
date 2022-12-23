cmake_minimum_required(VERSION 3.24)
project(AoC CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CXX_FLAGS_ASAN "-fsanitize=address,undefined -g -fno-omit-frame-pointer")
add_compile_definitions(INPUT_PATH="${CMAKE_SOURCE_DIR}/input")

include(CTest)

set(skip_days 16 19)
foreach(day RANGE 1 21)
    if (day IN_LIST skip_days)
        continue()
    endif()
    add_executable(${day} src/${day}.cpp)
    add_test(NAME "Day ${day}" COMMAND ${day})
endforeach()
