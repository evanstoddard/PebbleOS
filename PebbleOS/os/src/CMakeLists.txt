target_sources(app PRIVATE
  main.c
)

add_subdirectory(kernel)

target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
