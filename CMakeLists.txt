cmake_minimum_required (VERSION 3.1)
project (TinyJni)

set(JNI_DIRS $ENV{JAVA_HOME}/include)
if (WIN32)
	set(JNI_DIRS ${JNI_DIRS} $ENV{JAVA_HOME}/include/win32)
endif (WIN32)

if (APPLE)
	set(JNI_DIRS ${JNI_DIRS} $ENV{JAVA_HOME}/include/darwin)
endif (APPLE)

set (CMAKE_CXX_STANDARD 11)

include_directories(Include ${JNI_DIRS})

#add_subdirectory(Test)

file(GLOB CPP_SOURCES Sources/*.cpp)

file(GLOB HEADERS Include/*.h)

set(ALL_SOURCES ${CPP_SOURCES} ${HEADERS})

add_library(TinyJni ${ALL_SOURCES})