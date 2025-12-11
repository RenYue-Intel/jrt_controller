# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/runner/work/jrt_controller/jrt_controller/jbpf-protobuf/jbpf/3p/ubpf")
  file(MAKE_DIRECTORY "/home/runner/work/jrt_controller/jrt_controller/jbpf-protobuf/jbpf/3p/ubpf")
endif()
file(MAKE_DIRECTORY
  "/home/runner/work/jrt_controller/jrt_controller/build/ubpf/src/ubpf-build"
  "/home/runner/work/jrt_controller/jrt_controller/build/ubpf"
  "/home/runner/work/jrt_controller/jrt_controller/build/ubpf/tmp"
  "/home/runner/work/jrt_controller/jrt_controller/build/ubpf/src/ubpf-stamp"
  "/home/runner/work/jrt_controller/jrt_controller/build/ubpf/src"
  "/home/runner/work/jrt_controller/jrt_controller/build/ubpf/src/ubpf-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/runner/work/jrt_controller/jrt_controller/build/ubpf/src/ubpf-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/runner/work/jrt_controller/jrt_controller/build/ubpf/src/ubpf-stamp${cfgdir}") # cfgdir has leading slash
endif()
