# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/jrtc/jbpf-protobuf/jbpf/3p/ck"
  "/jrtc/build/jbpf-protobuf/jbpf/jbpf_libck-prefix/src/jbpf_libck-build"
  "/jrtc/build/jbpf-protobuf/jbpf/jbpf_libck-prefix"
  "/jrtc/build/jbpf-protobuf/jbpf/jbpf_libck-prefix/tmp"
  "/jrtc/build/jbpf-protobuf/jbpf/jbpf_libck-prefix/src/jbpf_libck-stamp"
  "/jrtc/build/jbpf-protobuf/jbpf/jbpf_libck-prefix/src"
  "/jrtc/build/jbpf-protobuf/jbpf/jbpf_libck-prefix/src/jbpf_libck-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/jrtc/build/jbpf-protobuf/jbpf/jbpf_libck-prefix/src/jbpf_libck-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/jrtc/build/jbpf-protobuf/jbpf/jbpf_libck-prefix/src/jbpf_libck-stamp${cfgdir}") # cfgdir has leading slash
endif()
