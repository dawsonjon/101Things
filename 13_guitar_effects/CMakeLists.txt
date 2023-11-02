add_executable(guitar_effects
  guitar_effects.cpp
  effects.cpp
)
pico_enable_stdio_usb(guitar_effects 1)
pico_enable_stdio_uart(guitar_effects 0)
pico_add_extra_outputs(guitar_effects)
target_include_directories(guitar_effects PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
if (PICO_CYW43_SUPPORTED)
target_link_libraries(guitar_effects pico_stdlib ui audio networking)
else()
target_link_libraries(guitar_effects pico_stdlib ui audio)
endif()
