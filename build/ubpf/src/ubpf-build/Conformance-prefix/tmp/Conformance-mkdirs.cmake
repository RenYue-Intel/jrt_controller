# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/jrtc/jbpf-protobuf/jbpf/3p/ubpf/external/bpf_conformance"
  "/jrtc/build/ubpf/src/ubpf-build/external/bpf_conformance"
  "/jrtc/build/ubpf/src/ubpf-build/Conformance-prefix"
  "/jrtc/build/ubpf/src/ubpf-build/Conformance-prefix/tmp"
  "/jrtc/build/ubpf/src/ubpf-build/Conformance-prefix/src/Conformance-stamp"
  "/jrtc/build/ubpf/src/ubpf-build/Conformance-prefix/src"
  "/jrtc/build/ubpf/src/ubpf-build/Conformance-prefix/src/Conformance-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/jrtc/build/ubpf/src/ubpf-build/Conformance-prefix/src/Conformance-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/jrtc/build/ubpf/src/ubpf-build/Conformance-prefix/src/Conformance-stamp${cfgdir}") # cfgdir has leading slash
endif()
