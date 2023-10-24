add_executable(tuner
  tuner.cpp
  half_band_filter2.cpp
)
pico_enable_stdio_usb(tuner 1)
pico_add_extra_outputs(tuner)
target_include_directories(tuner PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(tuner pico_stdlib audio)
