target_include_directories(app PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}
  device
)

add_subdirectory(applib)
add_subdirectory(flash_map)
add_subdirectory(kernel)
add_subdirectory(process_management)
add_subdirectory(resource)

