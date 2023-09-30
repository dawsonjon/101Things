cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(test
  test.cpp
  TCPServer.cpp
  PWMAudio.cpp
  ADCAudio.cpp
)
pico_enable_stdio_usb(test 1)
pico_enable_stdio_uart(test 1)
pico_add_extra_outputs(test)
target_include_directories(test PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(test pico_cyw43_arch_lwip_poll pico_stdlib hardware_pwm hardware_dma hardware_adc)
