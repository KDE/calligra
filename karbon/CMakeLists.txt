project(karbon)


#Set the correct compiler options
IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
   # 32 bit
   MESSAGE(STATUS "Karbon detected that you use a 32 bit processor.")
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 4)
   # 64 bit (well, anything else than 32 bit, but someone use something else than 32 or 64 bit ?)
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
   MESSAGE(STATUS "Karbon detected that you use an 64 bit processor. Added -fPIC to the CXX_FLAGS.")
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)

find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-karbon.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-karbon.h )


include_directories( 
		${KDE4_INCLUDE_DIR} 
		${QT_INCLUDES} 
		${KOFFICEUI_INCLUDES} 
		${KOPAINTER_INCLUDES}
		${KOPALETTE_INCLUDES}
		${CMAKE_BINARY_DIR}/karbon/
		${CMAKE_SOURCE_DIR}/karbon/core 
		${CMAKE_BINARY_DIR}/karbon/core 
		${CMAKE_SOURCE_DIR}/karbon/widgets 
		${CMAKE_BINARY_DIR}/karbon/widgets 
		${CMAKE_SOURCE_DIR}/karbon/render 
		${CMAKE_BINARY_DIR}/karbon/render 
		${CMAKE_SOURCE_DIR}/karbon/commands 
		${CMAKE_BINARY_DIR}/karbon/commands 
		${CMAKE_SOURCE_DIR}/karbon/dockers 
		${CMAKE_BINARY_DIR}/karbon/dockers 
		${CMAKE_SOURCE_DIR}/karbon/dialogs 
		${CMAKE_BINARY_DIR}/karbon/dialogs
		${FREETYPE_INCLUDE_DIR}
		${LCMS_INCLUDE_DIR})

add_subdirectory( commands )
add_subdirectory( visitors )
add_subdirectory( core )
add_subdirectory( shapes )
add_subdirectory( widgets )
add_subdirectory( dialogs )
add_subdirectory( dockers )
add_subdirectory( render )
add_subdirectory( tools )
add_subdirectory( plugins )
add_subdirectory( data )
add_subdirectory( pics )
add_subdirectory( templates )




########### next target ###############

set(karboncommon_LIB_SRCS 
   karbon_factory.cc 
   karbon_resourceserver.cc 
   karbon_part.cc 
#   karbon_part_iface.cc 
   karbon_view.cc 
#   karbon_view_iface.cc 
   karbon_drag.cpp 
   karbon_grid_data.cpp 
   karbon_tool_registry.cc 
   karbon_tool_factory.cc 
   vtoolcontroller.cc 
   vtool.cc )

kde4_automoc(${karboncommon_LIB_SRCS})

#kde4_add_dcop_skels(karboncommon_LIB_SRCS karbon_part_iface.h karbon_view_iface.h )

kde4_add_library(karboncommon SHARED ${karboncommon_LIB_SRCS})

target_link_libraries(karboncommon  ${KDE4_KDECORE_LIBS} kofficecore kofficeui kopalette kopainter kotext karbondockers karbonwidgets vpainter karboncore karbonshapes karboncommands karbonvisitors karbondialogs ${IMAGEMAGICK_LIBRARIES})

set_target_properties(karboncommon PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS karboncommon  DESTINATION ${LIB_INSTALL_DIR})


########### next target ###############

set(karbonpart_PART_SRCS karbon_factory_init.cc )

kde4_automoc(${karbonpart_PART_SRCS})

kde4_add_plugin(karbonpart WITH_PREFIX ${karbonpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonpart )

target_link_libraries(karbonpart  ${KDE4_KDECORE_LIBS} karboncommon )

install(TARGETS karbonpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(karbon_KDEINIT_SRCS main.cc )

kde4_automoc(${karbon_KDEINIT_SRCS})

kde4_add_kdeinit_executable( karbon ${karbon_KDEINIT_SRCS})

target_link_libraries(kdeinit_karbon  ${KDE4_KDECORE_LIBS} karboncommon )

install(TARGETS kdeinit_karbon  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( karbon kdeinit_karbon )
install(TARGETS karbon  DESTINATION ${BIN_INSTALL_DIR})
