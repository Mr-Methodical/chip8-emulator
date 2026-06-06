cmake_minimum_required(VERSION 3.10)
project(chip8)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

add_executable(chip8
  main.cpp
  chip8.cpp
  platform.cpp
  sound.cpp
)

target_include_directories(chip8 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(chip8 PRIVATE ${SDL2_LIBRARIES})
