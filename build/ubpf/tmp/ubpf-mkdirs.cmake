# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/jrtc/jbpf-protobuf/jbpf/3p/ubpf"
  "/jrtc/build/ubpf/src/ubpf-build"
  "/jrtc/build/ubpf"
  "/jrtc/build/ubpf/tmp"
  "/jrtc/build/ubpf/src/ubpf-stamp"
  "/jrtc/build/ubpf/src"
  "/jrtc/build/ubpf/src/ubpf-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/jrtc/build/ubpf/src/ubpf-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/jrtc/build/ubpf/src/ubpf-stamp${cfgdir}") # cfgdir has leading slash
endif()
