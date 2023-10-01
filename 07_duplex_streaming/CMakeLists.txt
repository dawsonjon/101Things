add_executable(duplex
  duplex.cpp
)
pico_enable_stdio_usb(duplex 1)
pico_add_extra_outputs(duplex)
target_include_directories(duplex PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(duplex audio networking)
