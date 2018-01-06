cmake_minimum_required(VERSION 2.8)
project(Overflow)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

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

option(BUILD_IOS "Enable iOS specific extensions" OFF)
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

# compile options
add_compile_options(-g -O2 -Wall)

# add source
add_subdirectory(src)

# jni bindings
option(BUILD_JNI "Enable jni" OFF)
if(BUILD_JNI)
  add_definitions(-D__IS_ANDROID__)
  set(LIBRARY_TYPE SHARED)
  set(UV_ANDROID ON)
endif()

# allow external projects
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# deps
ExternalProject_Add(
  glog
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/glog
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  )
ExternalProject_Add(
  cppcodec
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/cppcodec
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  )
ExternalProject_Add(
  libevent
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/libevent
  CMAKE_ARGS -DEVENT__DISABLE_TESTS=ON -DEVENT__DISABLE_REGRESS=ON -DEVENT__DISABLE_OPENSSL=ON -DEVENT__DISABLE_SAMPLES=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  )

# unit-tests
option(TESTS "Enable unit-tests" OFF)
if(TESTS)
  ExternalProject_Add(
    googletest
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/googletest
    CMAKE_ARGS -DBUILD_GMOCK=ON -DBUILD_GTEST=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/googletest
  )
  
  include(CTest)
  enable_testing()
  
  add_subdirectory(test)
endif()

