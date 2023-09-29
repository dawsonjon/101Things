# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/jon/pico/pico-sdk/tools/elf2uf2"
  "/home/jon/101Things/ExperimentsSDK/2_peer_to_peer/build/elf2uf2"
  "/home/jon/101Things/ExperimentsSDK/2_peer_to_peer/build/elf2uf2"
  "/home/jon/101Things/ExperimentsSDK/2_peer_to_peer/build/elf2uf2/tmp"
  "/home/jon/101Things/ExperimentsSDK/2_peer_to_peer/build/elf2uf2/src/ELF2UF2Build-stamp"
  "/home/jon/101Things/ExperimentsSDK/2_peer_to_peer/build/elf2uf2/src"
  "/home/jon/101Things/ExperimentsSDK/2_peer_to_peer/build/elf2uf2/src/ELF2UF2Build-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/jon/101Things/ExperimentsSDK/2_peer_to_peer/build/elf2uf2/src/ELF2UF2Build-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/jon/101Things/ExperimentsSDK/2_peer_to_peer/build/elf2uf2/src/ELF2UF2Build-stamp${cfgdir}") # cfgdir has leading slash
endif()
