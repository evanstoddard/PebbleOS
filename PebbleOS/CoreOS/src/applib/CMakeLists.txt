add_subdirectory(fonts)
add_subdirectory(graphics)
add_subdirectory(ui)

target_include_directories(app PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}
)
