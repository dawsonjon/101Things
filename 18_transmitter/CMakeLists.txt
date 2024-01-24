
add_executable(transmitter
  transmitter.cpp
  modulator.cpp
  adc.cpp
  nco.cpp
  pwm.cpp
  signal_generator.cpp
  half_band_filter.cpp
  half_band_filter2.cpp
  cordic.cpp
)

pico_generate_pio_header(transmitter ${CMAKE_CURRENT_LIST_DIR}/stream_bits.pio)

pico_enable_stdio_usb(transmitter 1)
pico_enable_stdio_uart(transmitter 0)
pico_add_extra_outputs(transmitter)
target_include_directories(transmitter PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(transmitter psu_mode pico_stdlib hardware_pio hardware_dma hardware_pwm hardware_adc)
