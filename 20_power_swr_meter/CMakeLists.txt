
add_executable(power_swr_meter
  power_swr_meter.cpp
)

pico_enable_stdio_usb(power_swr_meter 1)
pico_enable_stdio_uart(power_swr_meter 0)
pico_add_extra_outputs(power_swr_meter)
target_include_directories(power_swr_meter PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(power_swr_meter psu_mode pico_stdlib hardware_dma hardware_adc display)
