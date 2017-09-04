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

option(BUILD_MAC "Enable iOS/OSX specific extensions" OFF)
if(BUILD_IOS)
  add_definitions(-D__IS_IOS__)
  set(LIBRARY_TYPE STATIC)
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "macosx iphonesimulator iphoneos appletvsimulator appletvos")
  set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
  set(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.10")
  set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "8.0")
  set(CMAKE_XCODE_ATTRIBUTE_TVOS_DEPLOYMENT_TARGET "9.2")
  set(CMAKE_XCODE_ATTRIBUTE_SYMROOT "build")
  set(CMAKE_XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "$(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)")
  set(CMAKE_XCODE_ATTRIBUTE_CONFIGURATION_TEMP_DIR "$(PROJECT_TEMP_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)")
endif()

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
