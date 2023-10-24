add_executable(mic2speaker
  mic2speaker.cpp
)
pico_enable_stdio_usb(mic2speaker 1)
pico_add_extra_outputs(mic2speaker)
target_include_directories(mic2speaker PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
if (PICO_CYW43_SUPPORTED)
target_link_libraries(mic2speaker pico_cyw43_arch_lwip_poll pico_stdlib audio networking)
else()
target_link_libraries(mic2speaker pico_stdlib audio)
endif()
