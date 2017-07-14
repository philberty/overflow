include_directories(
  ${PROJECT_SOURCE_DIR}/ext/concurrentqueue
  ${PROJECT_SOURCE_DIR}/ext/libuv-cmake/libuv/include
  ${PROJECT_SOURCE_DIR}/ext/uvpp/include
  ${PROJECT_SOURCE_DIR}/ext/cppcodec/
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/test
  ${PROJECT_BINARY_DIR}/ext/glog
  ${PROJECT_SOURCE_DIR}/ext/glog/src
  )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -O2")

set(EXTRA_LINKS)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(EXTRA_LINKS "-lrt")
endif()
  
add_executable(testrunner
  testrunner.cc

  Scratch.cc

  ByteBufferTests.cc
  UrlTests.cc
  HelperTests.cc

  Util.h
  )

target_link_libraries(testrunner
  overflow
  uv
  glog
  gtest
  gmock
  ${CMAKE_DL_LIBS}
  ${EXTRA_LINKS}
  )

add_test(NAME testrunner
  COMMAND testrunner
)
