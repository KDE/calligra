project(karbon)


#Set the correct compiler options
IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
   # 32 bit
   MESSAGE(STATUS "Karbon detected that you use a 32 bit processor.")
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 4)
   # 64 bit (well, anything else than 32 bit, but someone use something else than 32 or 64 bit ?)
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
   MESSAGE(STATUS "Karbon detected that you use a 64 bit processor. Added -fPIC to the CXX_FLAGS.")
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-karbon.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-karbon.h )


include_directories( 
		${KDE4_INCLUDE_DIR} 
		${QT_INCLUDES} 
		${KOGUIUTILS_INCLUDES} 
		${CMAKE_BINARY_DIR}/karbon/
		${CMAKE_SOURCE_DIR}/karbon/core 
		${CMAKE_BINARY_DIR}/karbon/core 
		${CMAKE_SOURCE_DIR}/karbon/widgets 
		${CMAKE_BINARY_DIR}/karbon/widgets 
		${CMAKE_SOURCE_DIR}/karbon/commands
		${CMAKE_BINARY_DIR}/karbon/commands
		${CMAKE_SOURCE_DIR}/karbon/dockers 
		${CMAKE_BINARY_DIR}/karbon/dockers 
		${CMAKE_SOURCE_DIR}/karbon/dialogs 
		${CMAKE_BINARY_DIR}/karbon/dialogs
		${FREETYPE_INCLUDE_DIR}
		${LCMS_INCLUDE_DIR}
        ${KOTEXT_INCLUDES})

add_subdirectory( commands )
add_subdirectory( core )
add_subdirectory( widgets )
add_subdirectory( dialogs )
add_subdirectory( dockers )
add_subdirectory( plugins )
add_subdirectory( data )
add_subdirectory( pics )
add_subdirectory( templates )


########### next target ###############

set(karboncommon_LIB_SRCS 
   karbon_factory.cc 
#karbon_resourceserver.cc 
   KarbonPart.cpp
   KarbonView.cpp
   KarbonPrintJob.cpp
#   karbon_drag.cpp 
)

kde4_add_library(karboncommon SHARED ${karboncommon_LIB_SRCS})

target_link_libraries(karboncommon  ${KDE4_KDECORE_LIBS} komain koguiutils kotext karbondockers karbonwidgets karboncore karboncommands karbondialogs )

set_target_properties(karboncommon PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS karboncommon  DESTINATION ${LIB_INSTALL_DIR})

########### next target ###############

set(karbonpart_PART_SRCS karbon_factory_init.cc )

kde4_add_plugin(karbonpart WITH_PREFIX ${karbonpart_PART_SRCS})

target_link_libraries(karbonpart  ${KDE4_KDECORE_LIBS} karboncommon )

install(TARGETS karbonpart  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

set(karbon_KDEINIT_SRCS main.cc )

kde4_add_kdeinit_executable( karbon ${karbon_KDEINIT_SRCS})

target_link_libraries(kdeinit_karbon  ${KDE4_KDECORE_LIBS} karboncommon )

install(TARGETS kdeinit_karbon  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( karbon kdeinit_karbon )
install(TARGETS karbon  DESTINATION ${BIN_INSTALL_DIR})
