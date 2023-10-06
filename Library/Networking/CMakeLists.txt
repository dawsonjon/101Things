
add_library(networking
  TCPSocket.cpp
  dhcpserver/dhcpserver.c
  get_wifi.cpp
)

if ("${WIFI_SSID}" STREQUAL "")
    message("WIFI_SSID not defined, use USB/flash wifi configuration")
else ()
    message("Hard coding wifi SSID and password ${WIFI_SSID}")
    target_compile_definitions(networking PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    )
endif()

target_include_directories(networking PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
  )
target_link_libraries(networking pico_stdlib pico_cyw43_arch_lwip_poll hardware_flash pico_multicore)
