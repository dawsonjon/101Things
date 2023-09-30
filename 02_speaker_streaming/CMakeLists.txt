add_executable(stream2speaker
  stream2speaker.cpp
)
pico_enable_stdio_usb(stream2speaker 1)
pico_add_extra_outputs(stream2speaker)
target_include_directories(stream2speaker PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(stream2speaker audio networking)
