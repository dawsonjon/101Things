
add_executable(frequency_generator
  frequency_generator.cpp
  nco.cpp
)

pico_generate_pio_header(frequency_generator ${CMAKE_CURRENT_LIST_DIR}/stream_bits.pio)

pico_enable_stdio_usb(frequency_generator 1)
pico_enable_stdio_uart(frequency_generator 0)
pico_add_extra_outputs(frequency_generator)
target_include_directories(frequency_generator PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(frequency_generator pico_stdlib hardware_pio hardware_dma)
