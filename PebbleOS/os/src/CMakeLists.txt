target_sources(app PRIVATE
  main.c
)

add_subdirectory(ble)
add_subdirectory(kernel)
add_subdirectory(services)
add_subdirectory(utils)

target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
