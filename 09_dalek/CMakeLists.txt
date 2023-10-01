add_executable(dalek
  dalek.cpp
)
pico_enable_stdio_usb(dalek 1)
pico_add_extra_outputs(dalek)
target_include_directories(dalek PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(dalek pico_stdlib audio)
