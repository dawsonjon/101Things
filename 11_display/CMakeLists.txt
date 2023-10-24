add_executable(display_test
  display_test.cpp
)
pico_enable_stdio_usb(display_test 1)
pico_add_extra_outputs(display_test)
target_include_directories(display_test PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(display_test pico_stdlib display)
