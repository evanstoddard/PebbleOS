zephyr_library()

target_sources(native_simulator INTERFACE flash_sim_posix_api.c)
zephyr_library_sources(flash_sim.c)
