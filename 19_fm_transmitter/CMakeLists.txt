
add_executable(fm_transmitter
  transmitter.cpp
  modulator.cpp
  nco.cpp
)

pico_generate_pio_header(fm_transmitter ${CMAKE_CURRENT_LIST_DIR}/stream_bits.pio)

pico_enable_stdio_usb(fm_transmitter 1)
pico_enable_stdio_uart(fm_transmitter 0)
pico_add_extra_outputs(fm_transmitter)
target_include_directories(fm_transmitter PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(fm_transmitter pico_stdlib hardware_pio hardware_dma hardware_pwm hardware_adc)
