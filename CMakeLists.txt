cmake_minimum_required(VERSION 3.13)

PROJECT(blogcpp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

######################################################################
# OS
######################################################################
IF (UNIX)
	ADD_DEFINITIONS(-DUNIX)

	############################################################################
	# CPP FLAGS
	############################################################################
	SET(CMAKE_CXX_FLAGS "-Wall -Wno-deprecated-declarations ${CMAKE_CXX_FLAGS} -Werror=switch")

	############################################################################
	# RPATH
	# https://cmake.org/Wiki/CMake_RPATH_handling
	############################################################################
	# use, i.e. don't skip the full RPATH for the build tree
	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

	# when building, don't use the install RPATH already
	# (but later on when installing)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

	SET(CMAKE_INSTALL_RPATH "${TARGET_LIB_PATH}")

	# add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

	# the RPATH to be used when installing, but only if it's not a system directory
	LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${TARGET_LIB_PATH}" isSystemDir)
	IF("${isSystemDir}" STREQUAL "-1")
		SET(CMAKE_INSTALL_RPATH "${TARGET_LIB_PATH}")
	ENDIF("${isSystemDir}" STREQUAL "-1")

	IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		ADD_DEFINITIONS(-DLINUX)
		
		SET(LINUX TRUE)
		SET(FREEBSD FALSE)
		SET(DARWIN FALSE)	
	ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	
	IF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
		ADD_DEFINITIONS(-DFREEBSD)
		
		SET(LINUX FALSE)
		SET(FREEBSD TRUE)
		SET(DARWIN FALSE)
	ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	
	IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		ADD_DEFINITIONS(-DDARWIN)

        SET(LINUX FALSE)
		SET(FREEBSD FALSE)
		SET(DARWIN TRUE)
	ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
ENDIF (UNIX)
 
IF (WIN32)
IF (MSVC)
	ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)

	############################################################################
	# CPP FLAGS
	############################################################################
	SET(LIB_CXX_FLAGS "")

	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)

	############################################################################
	# PATHS for targets
	############################################################################
	SET(TARGET_BIN_PATH "${CMAKE_INSTALL_PREFIX}/")
	SET(TARGET_LIB_PATH "${CMAKE_INSTALL_PREFIX}/")
ELSE (MSVC)
	ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)

	############################################################################
	# CPP FLAGS
	############################################################################
	SET(CMAKE_CXX_FLAGS   "--std=c++14 -Wall -Wno-deprecated-declarations ${CMAKE_CXX_FLAGS} -Werror=switch")
	SET(LIB_CXX_FLAGS "")

	ADD_DEFINITIONS(-DNOEXCEPT=noexcept)

	############################################################################
	# PATHS for targets
	############################################################################
	SET(TARGET_BIN_PATH "${CMAKE_INSTALL_PREFIX}/")
	SET(TARGET_LIB_PATH "${CMAKE_INSTALL_PREFIX}/")

ENDIF (MSVC)
ENDIF (WIN32)

# Downloading the Conan wrapper if required:
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake" 
                 TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

# Fetching dependencies:
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

add_subdirectory(src)
