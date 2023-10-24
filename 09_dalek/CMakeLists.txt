add_executable(dalek
  dalek.cpp
)
pico_enable_stdio_usb(dalek 1)
pico_add_extra_outputs(dalek)
target_include_directories(dalek PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
if (PICO_CYW43_SUPPORTED)
target_link_libraries(dalek pico_cyw43_arch_lwip_poll pico_stdlib audio networking)
else()
target_link_libraries(dalek pico_stdlib audio)
endif()
