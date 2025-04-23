# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "AirQualityMonitor_autogen"
  "CMakeFiles\\AirQualityMonitor_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AirQualityMonitor_autogen.dir\\ParseCache.txt"
  )
endif()
