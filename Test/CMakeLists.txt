cmake_minimum_required (VERSION 3.1)
project (TestTinyJni)

file(GLOB CPP_SOURCES Src/*.cpp)

file(GLOB HEADERS Include/*.h)

set(LINK_LIBRARIES TinyJni gtest)

set(JNI_DIRS ${JAVA_HOME}/include)

if (WIN32)

	set(JNI_DIRS ${JAVA_HOME}/include ${JAVA_HOME}/include/win32)
	
	set(CPP_SOURCES ${CPP_SOURCES} Src/Win32/TJJavaVMInitializer.cpp)
	
	if (MSVC)
		SET( CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /MTd" CACHE STRING "MSVC MT flags " FORCE)
		SET( CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} /MT" CACHE STRING "MSVC MT flags " FORCE)
	endif (MSVC)
elseif (UNIX)
	if (APPLE)
		execute_process(COMMAND "/usr/libexec/java_home" OUTPUT_VARIABLE JAVA_HOME)

		# strip trailing newline
		string(REGEX REPLACE "\n$" "" JAVA_HOME "${JAVA_HOME}")

		set(JNI_DIRS ${JAVA_HOME}/include ${JAVA_HOME}/include/darwin)
		
		set(CPP_SOURCES ${CPP_SOURCES} Src/MacOs/TJJavaVMInitializer.cpp)
	
	else()
		
		set(JNI_DIRS ${JAVA_HOME}/include ${JAVA_HOME}/include/linux)
		
		set(CPP_SOURCES ${CPP_SOURCES} Src/Linux/TJJavaVMInitializer.cpp)
		
		set(LINK_LIBRARIES ${LINK_LIBRARIES} dl)
	
	endif (APPLE)
endif(WIN32)

include_directories(Include ${JNI_DIRS} gtest/googletest/include)

set(ALL_SOURCES ${CPP_SOURCES} ${HEADERS})

set(BUILD_GTEST 1)

add_subdirectory(gtest)

add_executable(TestTinyJni ${ALL_SOURCES})

target_link_libraries(TestTinyJni ${LINK_LIBRARIES})
