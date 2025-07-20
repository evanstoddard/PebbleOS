add_subdirectory(fonts)
add_subdirectory(graphics)
add_subdirectory(ui)
add_subdirectory(memory)

target_sources(app PRIVATE
  app.c
)

target_include_directories(app PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}
)
