include_directories(
  ${CMAKE_INSTALL_PREFIX}/include
  ${PROJECT_BINARY_DIR}/src
  )
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

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
  ByteBuffer.h
  InterleavedTcpTransport.h
  RtspController.h
  TransportController.h
  Options.h
  SessionDescriptionFactory.h
  Teardown.h
  Describe.h
  Pause.h
  Play.h
  Setup.h
  Rtsp.h
  IRtspDelegate.h
  RtspFactory.h
  SessionDescription.h
  Transport.h
  DescribeResponse.h
  ITransportDelegate.h
  SessionDescriptionV0.h
  Url.h
  H264Depacketizer.h
  MJPEGDepacketizer.h
  Response.h
  RtspResponse.h
  Helpers.h
  MP4VDepacketizer.h
  RtpPacket.h
  RtspWanClient.h
  SetupResponse.h
)

add_library(overflow 
  RtspWanClient.cc
  RtspController.cc
  TransportController.cc
  InterleavedTcpTransport.cc
  Response.cc
  RtspResponse.cc
  RtspFactory.cc
  Rtsp.cc
  Url.cc
  RtpPacket.cc
  Helpers.cc
  ByteBuffer.cc
  RtspFactory.cc
  SessionDescriptionFactory.cc
  SessionDescriptionV0.cc
  SetupResponse.cc
  H264Depacketizer.cc
  MP4VDepacketizer.cc
  MJPEGDepacketizer.cc

  ${OVERFLOW_HEADERS}
  ${JNI_SOURCES}
)

if(BUILD_JNI)
  target_link_libraries (overflow ${JNI_LIBS} event glog)
else()
  target_link_libraries (overflow event glog)
endif()

install(TARGETS overflow
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(FILES ${OVERFLOW_HEADERS}
  DESTINATION include/overflow
  )
