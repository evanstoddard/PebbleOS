cmake_minimum_required(VERSION 3.20)

project(font_viewer)

find_package(SDL2 REQUIRED)

add_executable(font_viewer
  src/main.c
)

target_include_directories(font_viewer PRIVATE
  ../../PebbleOS/CoreOS/src/fw/applib/fonts/
)


target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2::SDL2main)
