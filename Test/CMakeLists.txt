cmake_minimum_required (VERSION 3.1)
project (TestTinyJni)

file(GLOB CPP_SOURCES Src/*.cpp)

file(GLOB HEADERS Include/*.h)

set(JNI_DIRS $ENV{JAVA_HOME}/include)

if (WIN32)
	set(JNI_DIRS ${JNI_DIRS} $ENV{JAVA_HOME}/include/win32)
	set(CPP_SOURCES ${CPP_SOURCES} Src/Win32/TJJavaVMInitializer.cpp)
	if (MSVC)
		SET( CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /MTd" CACHE STRING "MSVC MT flags " FORCE)
		SET( CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} /MT" CACHE STRING "MSVC MT flags " FORCE)
	endif (MSVC)
endif (WIN32)

if (APPLE)
	set(JNI_DIRS ${JNI_DIRS} $ENV{JAVA_HOME}/include/darwin)
	set(CPP_SOURCES ${CPP_SOURCES} Src/MacOs/TJJavaVMInitializer.cpp)
endif (APPLE)

include_directories(Include ${JNI_DIRS} gtest/googletest/include)

set(ALL_SOURCES ${CPP_SOURCES} ${HEADERS})

set(BUILD_GTEST 1)

add_subdirectory(gtest)

add_executable(TestTinyJni ${ALL_SOURCES})

target_link_libraries(TestTinyJni TinyJni gtest)
