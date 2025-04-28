# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\DATN_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\DATN_autogen.dir\\ParseCache.txt"
  "DATN_autogen"
  )
endif()
