cmake_minimum_required (VERSION 2.8.2)
project (TestTinyJni)

set(JNI_DIRS $ENV{JAVA_HOME}/include $ENV{JAVA_HOME}/include/win32)

include_directories(Include ${JNI_DIRS})

file(GLOB CPP_SOURCES Src/*.cpp)

file(GLOB HEADERS Include/*.h)

set(ALL_SOURCES ${CPP_SOURCES} ${HEADERS})

include(ExternalProject)
ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG           master
  SOURCE_DIR        "${CMAKE_BINARY_DIR}/googletest-src"
  BINARY_DIR        "${CMAKE_BINARY_DIR}/googletest-build"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

add_executable(TestTinyJni ${ALL_SOURCES})