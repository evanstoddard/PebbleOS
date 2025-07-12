target_sources(app PRIVATE
  applib_malloc.c
)

target_link_directories(app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
