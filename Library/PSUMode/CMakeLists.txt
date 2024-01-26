add_library(psu_mode
  psu_mode.cpp
)
target_include_directories(psu_mode PUBLIC ${CMAKE_CURRENT_LIST_DIR})
if (PICO_CYW43_SUPPORTED)
target_link_libraries(psu_mode pico_stdlib pico_cyw43_arch_lwip_poll networking)
else()
target_link_libraries(psu_mode pico_stdlib)
endif()
