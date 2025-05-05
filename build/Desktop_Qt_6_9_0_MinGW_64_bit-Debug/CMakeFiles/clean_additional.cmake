# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "AirQualityMonitorTests_autogen"
  "AirQualityMonitor_autogen"
  "AirQualityMonitor_lib_autogen"
  "CMakeFiles\\AirQualityMonitorTests_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AirQualityMonitorTests_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\AirQualityMonitor_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AirQualityMonitor_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\AirQualityMonitor_lib_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AirQualityMonitor_lib_autogen.dir\\ParseCache.txt"
  "docs\\html"
  )
endif()
