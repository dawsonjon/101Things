add_library(audio
  ADCAudio.cpp
  PWMAudio.cpp
)
target_include_directories(audio PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(audio pico_stdlib hardware_adc hardware_pwm hardware_dma)
