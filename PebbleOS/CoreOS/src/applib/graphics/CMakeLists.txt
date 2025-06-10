target_sources(app PRIVATE
  framebuffer.c
  gbitmap.c
  gcontext.c
  graphics.c
  text.c
  text_layout.c
  text_resources.c
)

add_subdirectory(types)

target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
