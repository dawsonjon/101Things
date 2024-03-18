
add_executable(transmitter_serial_example_mixer
  transmitter_serial_example.cpp
  transmit.cpp
  modulator.cpp
  adc.cpp
  nco.cpp
  pwm.cpp
  signal_generator.cpp
  half_band_filter.cpp
  half_band_filter2.cpp
  cordic.cpp
)

target_compile_definitions(transmitter_serial_example_mixer PUBLIC BALANCED)
pico_generate_pio_header(transmitter_serial_example_mixer ${CMAKE_CURRENT_LIST_DIR}/stream_bits.pio)
pico_enable_stdio_usb(transmitter_serial_example_mixer 1)
pico_enable_stdio_uart(transmitter_serial_example_mixer 0)
pico_add_extra_outputs(transmitter_serial_example_mixer)
target_include_directories(transmitter_serial_example_mixer PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(transmitter_serial_example_mixer psu_mode pico_stdlib hardware_pio hardware_dma hardware_pwm hardware_adc)

add_executable(transmitter_microphone_example_mixer
  transmitter_microphone_example.cpp
  transmit.cpp
  modulator.cpp
  adc.cpp
  nco.cpp
  pwm.cpp
  signal_generator.cpp
  half_band_filter.cpp
  half_band_filter2.cpp
  cordic.cpp
)

target_compile_definitions(transmitter_microphone_example_mixer PUBLIC BALANCED)
pico_generate_pio_header(transmitter_microphone_example_mixer ${CMAKE_CURRENT_LIST_DIR}/stream_bits.pio)
pico_enable_stdio_usb(transmitter_microphone_example_mixer 1)
pico_enable_stdio_uart(transmitter_microphone_example_mixer 0)
pico_add_extra_outputs(transmitter_microphone_example_mixer)
target_include_directories(transmitter_microphone_example_mixer PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(transmitter_microphone_example_mixer psu_mode pico_stdlib hardware_pio hardware_dma hardware_pwm hardware_adc)

add_executable(transmitter_serial_example_polar_amplifier
  transmitter_serial_example.cpp
  transmit.cpp
  modulator.cpp
  adc.cpp
  nco.cpp
  pwm.cpp
  signal_generator.cpp
  half_band_filter.cpp
  half_band_filter2.cpp
  cordic.cpp
)

pico_generate_pio_header(transmitter_serial_example_polar_amplifier ${CMAKE_CURRENT_LIST_DIR}/stream_bits.pio)
pico_enable_stdio_usb(transmitter_serial_example_polar_amplifier 1)
pico_enable_stdio_uart(transmitter_serial_example_polar_amplifier 0)
pico_add_extra_outputs(transmitter_serial_example_polar_amplifier)
target_include_directories(transmitter_serial_example_polar_amplifier PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(transmitter_serial_example_polar_amplifier psu_mode pico_stdlib hardware_pio hardware_dma hardware_pwm hardware_adc)

add_executable(transmitter_microphone_example_polar_amplifier
  transmit.cpp
  transmitter_microphone_example.cpp
  modulator.cpp
  adc.cpp
  nco.cpp
  pwm.cpp
  signal_generator.cpp
  half_band_filter.cpp
  half_band_filter2.cpp
  cordic.cpp
)

pico_generate_pio_header(transmitter_microphone_example_polar_amplifier ${CMAKE_CURRENT_LIST_DIR}/stream_bits.pio)
pico_enable_stdio_usb(transmitter_microphone_example_polar_amplifier 1)
pico_enable_stdio_uart(transmitter_microphone_example_polar_amplifier 0)
pico_add_extra_outputs(transmitter_microphone_example_polar_amplifier)
target_include_directories(transmitter_microphone_example_polar_amplifier PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(transmitter_microphone_example_polar_amplifier psu_mode pico_stdlib hardware_pio hardware_dma hardware_pwm hardware_adc)
