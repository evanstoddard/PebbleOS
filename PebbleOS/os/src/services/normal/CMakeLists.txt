target_sources(app PRIVATE
  normal_services.c
)

add_subdirectory(filesystem)
add_subdirectory(kvs_storage)
