project( RtfReader )

cmake_minimum_required(VERSION 3.2)

include(GenerateExportHeader)

find_package(Qt6 5.2.0 REQUIRED Gui)

add_subdirectory( src )
