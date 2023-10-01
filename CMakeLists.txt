cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

include(cmake/clang-cxx-dev-tools.cmake)

add_subdirectory(01_microphone_streaming)
add_subdirectory(02_speaker_streaming)
add_subdirectory(03_microphone_loopback)
add_subdirectory(07_duplex_streaming)
add_subdirectory(08_voice_changer)
add_subdirectory(09_dalek)
add_subdirectory(Library)


