#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ubpf::ubpf" for configuration ""
set_property(TARGET ubpf::ubpf APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(ubpf::ubpf PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "/jrtc/out/lib/libubpf.a"
  )

list(APPEND _cmake_import_check_targets ubpf::ubpf )
list(APPEND _cmake_import_check_files_for_ubpf::ubpf "/jrtc/out/lib/libubpf.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
