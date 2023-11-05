add_executable(mic2stream
  mic2stream.cpp
)
pico_enable_stdio_usb(mic2stream 1)
pico_enable_stdio_uart(mic2stream 0)
pico_add_extra_outputs(mic2stream)
target_include_directories(mic2stream PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(mic2stream pico_stdlib audio networking psu_mode)
