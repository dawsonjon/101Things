add_executable(capture_audio
  capture_audio.cpp
)
pico_enable_stdio_usb(capture_audio 1)
pico_enable_stdio_uart(capture_audio 0)
pico_add_extra_outputs(capture_audio)
target_include_directories(capture_audio PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
if (PICO_CYW43_SUPPORTED)
target_link_libraries(capture_audio pico_stdlib audio networking)
else()
target_link_libraries(capture_audio pico_stdlib audio)
endif()
