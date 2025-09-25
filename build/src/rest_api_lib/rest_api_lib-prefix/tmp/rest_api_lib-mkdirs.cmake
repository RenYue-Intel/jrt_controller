# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/jrtc/build/src/rest_api_lib/rest_api_lib-prefix/src/rest_api_lib"
  "/jrtc/src/rest_api_lib"
  "/jrtc/build/src/rest_api_lib/rest_api_lib-prefix"
  "/jrtc/build/src/rest_api_lib/rest_api_lib-prefix/tmp"
  "/jrtc/build/src/rest_api_lib/rest_api_lib-prefix/src/rest_api_lib-stamp"
  "/jrtc/build/src/rest_api_lib/rest_api_lib-prefix/src"
  "/jrtc/build/src/rest_api_lib/rest_api_lib-prefix/src/rest_api_lib-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/jrtc/build/src/rest_api_lib/rest_api_lib-prefix/src/rest_api_lib-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/jrtc/build/src/rest_api_lib/rest_api_lib-prefix/src/rest_api_lib-stamp${cfgdir}") # cfgdir has leading slash
endif()
