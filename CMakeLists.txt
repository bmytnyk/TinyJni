cmake_minimum_required (VERSION 3.1)
project (TinyJni)

if (WIN32)
	set(JAVA_HOME $ENV{JAVA_HOME})
	set(JNI_DIRS ${JAVA_HOME}/include ${JAVA_HOME}/include/win32)
elseif(UNIX)
	if (APPLE)
		execute_process(COMMAND "/usr/libexec/java_home" OUTPUT_VARIABLE JAVA_HOME)

		# strip trailing newline
		string(REGEX REPLACE "\n$" "" JAVA_HOME "${JAVA_HOME}")

		set(JNI_DIRS ${JAVA_HOME}/include ${JAVA_HOME}/include/darwin)
	else()
		set(JAVA_HOME $ENV{JAVA_HOME})

		set(JNI_DIRS ${JAVA_HOME}/include ${JAVA_HOME}/include/linux)
	endif(APPLE)
endif(WIN32)

set (CMAKE_CXX_STANDARD 11)

include_directories(Include ${JNI_DIRS})

add_subdirectory(Test)

file(GLOB CPP_SOURCES Sources/*.cpp)

file(GLOB HEADERS Include/*.h)

set(ALL_SOURCES ${CPP_SOURCES} ${HEADERS})

add_library(TinyJni ${ALL_SOURCES})
