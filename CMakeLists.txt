cmake_minimum_required (VERSION 2.8.2)
project (TinyJni)

set(JNI_DIRS $ENV{JAVA_HOME}/include $ENV{JAVA_HOME}/include/win32)

include_directories(Include ${JNI_DIRS})

#add_subdirectory(Test)

file(GLOB CPP_SOURCES Sources/*.cpp)

file(GLOB HEADERS Include/*.h)

set(ALL_SOURCES ${CPP_SOURCES} ${HEADERS})

add_library(TinyJni ${ALL_SOURCES})