# Copyright (c) 2014 Stuart Dickson <stuartmd@kogmbh.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# - Try to find the Fftw3 Library
# Once done this will define
#
#  STEAMWORKS_FOUND - system has fftw3
#  STEAMWORKS_INCLUDE_DIRS - the fftw3 include directories
#  STEAMWORKS_LIBRARIES - the libraries needed to use Steamworks
#  STEAMWORKS_SHARED_LIBRARY_NAME - Name of DLL we need to distribute (excludes extension)
#  STEAMWORKS_SHARED_LIBRARY_PATH - Full path of DLL we need to distribute
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

message(STATUS "Steamworks - Finding include dirs")
find_path(STEAMWORKS_INCLUDE_DIR
    NAMES steam/steam_api.h
    #HINTS ${FFTW3_PKGCONF_INCLUDE_DIRS} ${FFTW3_PKGCONF_INCLUDEDIR}
    #PATH_SUFFIXES fftw3
)

IF (WIN32)
	IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(STEAMWORKS_SHARED_LIBRARY_NAME steam_api64)		
	ELSE (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(STEAMWORKS_SHARED_LIBRARY_NAME steam_api)		
	ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 8)
	
	find_library(STEAMWORKS_LIBRARY
		NAMES ${STEAMWORKS_SHARED_LIBRARY_NAME}
	)
	find_file(STEAMWORKS_SHARED_LIBRARY_PATH
		NAMES ${STEAMWORKS_SHARED_LIBRARY_NAME}.dll
	)
ELSE (WIN32)
	find_library(STEAMWORKS_LIBRARY
		NAMES steam_api
	)
ENDIF (WIN32)
set(STEAMWORKS_FOUND OFF)

IF (STEAMWORKS_LIBRARY)
	IF (STEAMWORKS_INCLUDE_DIR)
		set(STEAMWORKS_LIBRARIES ${STEAMWORKS_LIBRARY})
		set(STEAMWORKS_INCLUDE_DIRS ${STEAMWORKS_INCLUDE_DIR})
		set(STEAMWORKS_SHARED_LIBRARY_NAME ${STEAMWORKS_SHARED_LIBRARY_NAME})
		set(STEAMWORKS_SHARED_LIBRARY_PATH ${STEAMWORKS_SHARED_LIBRARY_PATH})
		set(STEAMWORKS_FOUND ON)
	ENDIF (STEAMWORKS_INCLUDE_DIR)
ENDIF (STEAMWORKS_LIBRARY)


