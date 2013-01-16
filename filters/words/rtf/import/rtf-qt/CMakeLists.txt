project( RtfReader )

cmake_minimum_required(VERSION 2.8.0)

#cmake_policy CMP0017 was introduced in version 2.8.4
if(${CMAKE_VERSION} VERSION_GREATER 2.8.3)
    cmake_policy(SET CMP0017 NEW)
endif()

find_package( Qt4 REQUIRED )

add_subdirectory( src )
