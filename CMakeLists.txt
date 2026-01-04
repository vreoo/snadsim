cmake_minimum_required(VERSION 3.16)
project(falling_sand C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(falling_sand
    src/main.c
)

target_include_directories(falling_sand PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(falling_sand PRIVATE
    ${SDL2_LIBRARIES}
)

target_compile_options(falling_sand PRIVATE
    -Wall -Wextra -Wpedantic
)