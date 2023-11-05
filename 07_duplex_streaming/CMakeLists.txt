add_executable(duplex_server
  duplex.cpp
)
target_compile_definitions(duplex_server PRIVATE SERVER)
pico_enable_stdio_usb(duplex_server 1)
pico_add_extra_outputs(duplex_server)
target_include_directories(duplex_server PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(duplex_server audio networking psu_mode)

add_executable(duplex_client
  duplex.cpp
)
target_compile_definitions(duplex_client PRIVATE CLIENT)
pico_enable_stdio_usb(duplex_client 1)
pico_add_extra_outputs(duplex_client)
target_include_directories(duplex_client PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(duplex_client audio networking psu_mode)

add_executable(duplex_ap_server
  duplex.cpp
)
target_compile_definitions(duplex_ap_server PRIVATE ACCESS_POINT_SERVER)
pico_enable_stdio_usb(duplex_ap_server 1)
pico_add_extra_outputs(duplex_ap_server)
target_include_directories(duplex_ap_server PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries(duplex_ap_server audio networking psu_mode)

