include_directories(
  ${PROJECT_SOURCE_DIR}/ext/concurrentqueue
  ${PROJECT_SOURCE_DIR}/ext/libuv-cmake/libuv/include
  ${PROJECT_SOURCE_DIR}/ext/uvpp/include
  ${PROJECT_SOURCE_DIR}/ext/uri/include
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_BINARY_DIR}/ext/glog
  ${PROJECT_SOURCE_DIR}/ext/glog/src
  ${PROJECT_SOURCE_DIR}/ext/cppcodec/
  )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(JNI_SOURCES)
set(JNI_LIBS)
if(BUILD_JNI)

  set(ANDROID_NDK_PLATFORM_LIBS)
  if(ANDROID_ABI STREQUAL "armeabi")    
    set(ANDROID_NDK_PLATFORM_LIBS
      ${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-arm/usr/lib
      )
  elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(ANDROID_NDK_PLATFORM_LIBS
      ${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-arm/usr/lib
      )
  elseif(ANDROID_ABI STREQUAL "arm64-v8a")
    set(ANDROID_NDK_PLATFORM_LIBS
      ${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-arm64/usr/lib
      )
  else()
    set(ANDROID_NDK_PLATFORM_LIBS
      ${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-${ANDROID_ABI}/usr/lib
      )
  endif()
  
  find_library(ANDROID_LOG
    NAMES log
    PATHS ${ANDROID_NDK_PLATFORM_LIBS}
    NO_DEFAULT_PATH
    )
  message(STATUS "${ANDROID_LOG}")

  set(JNI_LIBS "${ANDROID_LOG}")
  set(JNI_SOURCES
    ${PROJECT_SOURCE_DIR}/src/jni/RtspClientWrapper.cc
    ${PROJECT_SOURCE_DIR}/src/jni/RtspClientWrapper.h
    ${PROJECT_SOURCE_DIR}/src/jni/RtspDelegateWrapper.h)
endif()

set(OVERFLOW_HEADERS
  RtspWanClient.h
  IRtspDelegate.h
  ITransportDelegate.h

  Rtsp.h
  Describe.h
  Setup.h
  Play.h
  Pause.h
  Options.h
  Teardown.h

  RtspFactory.h

  InterleavedTcpTransport.h

  Response.h
  RtspResponse.h
  DescribeResponse.h
  SetupResponse.h

  SessionDescription.h
  SessionDescriptionV0.h
  SessionDescriptionFactory.h

  H264Depacketizer.h
  MP4VDepacketizer.h
  MJPEGDepacketizer.h
 
  RtpPacket.h
  ByteBuffer.h
  Helpers.h
)

add_library(overflow 
  RtspWanClient.cc
  InterleavedTcpTransport.cc
  Response.cc
  RtspResponse.cc
  RtspFactory.cc
  Url.cc
  RtpPacket.cc
  Helpers.cc
  ByteBuffer.cc
  SessionDescriptionFactory.cc
  SessionDescriptionV0.cc
  SetupResponse.cc
  H264Depacketizer.cc
  MP4VDepacketizer.cc
  MJPEGDepacketizer.cc

  ${OVERFLOW_HEADERS}
  ${JNI_SOURCES}

  $<TARGET_OBJECTS:uv_obj>
  $<TARGET_OBJECTS:glog_obj>
  )

add_dependencies(overflow uv glog)

if(BUILD_JNI)
  target_link_libraries (overflow ${JNI_LIBS})
endif()

install(TARGETS overflow
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(FILES ${OVERFLOW_HEADERS}
  DESTINATION include/overflow
  )
