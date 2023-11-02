
add_library(ui
  ui.cpp
)

target_include_directories(ui PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(ui display pico_stdlib hardware_i2c)
