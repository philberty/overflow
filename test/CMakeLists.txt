include_directories(
  ${CMAKE_INSTALL_PREFIX}/include
  ${PROJECT_BINARY_DIR}/src
  )
link_directories(${CMAKE_INSTALL_PREFIX}/lib)
 
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
  gtest
  gmock
  )

add_test(NAME testrunner
  COMMAND testrunner
)
