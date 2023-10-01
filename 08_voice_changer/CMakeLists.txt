add_executable(voice_changer
  voice_changer.cpp
  half_band_filter2.cpp
)
pico_enable_stdio_usb(voice_changer 1)
pico_add_extra_outputs(voice_changer)
target_include_directories(voice_changer PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(voice_changer pico_stdlib audio)
