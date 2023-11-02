add_executable(timer
  timer.cpp
)
pico_enable_stdio_usb(timer 1)
pico_enable_stdio_uart(timer 0)
pico_add_extra_outputs(timer)
target_include_directories(timer PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(timer pico_stdlib audio display ui)
