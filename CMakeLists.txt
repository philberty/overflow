cmake_minimum_required(VERSION 2.8)
project(Overflow)

include(TestBigEndian)
test_big_endian(TEST_ENDIAN)
message(STATUS "ENDIAN: ${TEST_ENDIAN}")
set(OVERFLOW_IS_BIG_ENDIAN ${TEST_ENDIAN})

EXEC_PROGRAM(./version.sh "${PROJECT_SOURCE_DIR}"
  ARGS --version
  OUTPUT_VARIABLE VERSION_STRING
  RETURN_VALUE)
EXEC_PROGRAM(./version.sh "${PROJECT_SOURCE_DIR}"
  ARGS --builder
  OUTPUT_VARIABLE BUILDER
  RETURN_VALUE)
EXEC_PROGRAM(./version.sh "${PROJECT_SOURCE_DIR}"
  ARGS --date
  OUTPUT_VARIABLE BUILD_DATE
  RETURN_VALUE)
configure_file (
  "${PROJECT_SOURCE_DIR}/src/Config.h.in"
  "${PROJECT_BINARY_DIR}/src/Config.h"
  )

# add source
add_subdirectory(src)

# jni bindings
option(BUILD_JNI "Enable jni" OFF)
if(BUILD_JNI)
  add_definitions(-D__IS_ANDROID__)
  set(LIBRARY_TYPE SHARED)
  set(UV_ANDROID ON)
endif()

# configure deps
set(WITH_GFLAGS OFF CACHE BOOL "Use gflags")
set(build_tests OFF CACHE BOOL "Build tests (run as 'test' target)")
set(BUILD_GMOCK ON)
set(BUILD_GTEST ON)

# unit-tests
option(TESTS "Enable unit-tests" OFF)
if(TESTS)
  include(CTest)
  enable_testing()
  
  add_subdirectory(test)
endif()

# deps
add_subdirectory(./ext/googletest)
add_subdirectory(./ext/libuv-cmake)
add_subdirectory(./ext/uvpp)
add_subdirectory(./ext/glog)
add_subdirectory(./ext/cppcodec)
