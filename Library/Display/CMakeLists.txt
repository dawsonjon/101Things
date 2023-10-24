
add_library(display
  ssd1306.c
)

target_include_directories(display PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(display pico_stdlib hardware_i2c)
