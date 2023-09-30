add_library(networking
  TCPServer.cpp
)
target_include_directories(networking PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(networking pico_stdlib pico_cyw43_arch_lwip_poll)
