
add_executable(inclinometer
  inclinometer.cpp
  st7735.cpp
  mpu6050.cpp
)

pico_enable_stdio_usb(inclinometer 1)
pico_enable_stdio_uart(inclinometer 0)
pico_add_extra_outputs(inclinometer)
target_include_directories(inclinometer PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(inclinometer psu_mode pico_stdlib hardware_spi hardware_i2c pico_multicore)
