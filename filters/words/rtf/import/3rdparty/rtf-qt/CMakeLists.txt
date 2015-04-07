project( RtfReader )

cmake_minimum_required(VERSION 2.8.12)

include(GenerateExportHeader)

#cmake_policy CMP0017 was introduced in version 2.8.4
if(${CMAKE_VERSION} VERSION_GREATER 2.8.3)
    cmake_policy(SET CMP0017 NEW)
endif()

find_package(Qt5 5.2.0 REQUIRED Gui)

add_subdirectory( src )
