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
        ${QT_INCLUDES} 
        ${KDE4_INCLUDE_DIR} 
        ${KOGUIUTILS_INCLUDES} 
        ${FLAKE_INCLUDES}
        ${KOTEXT_INCLUDES}
        ${CMAKE_SOURCE_DIR}/karbon/ui
        ${CMAKE_SOURCE_DIR}/karbon/common
        ${CMAKE_SOURCE_DIR}/karbon/common/commands
)

add_subdirectory( common )
add_subdirectory( ui )
add_subdirectory( plugins )
add_subdirectory( data )
add_subdirectory( pics )
add_subdirectory( templates )

########### next target ###############

set(karbonpart_PART_SRCS karbon_factory_init.cc )

kde4_add_plugin(karbonpart WITH_PREFIX ${karbonpart_PART_SRCS})

target_link_libraries(karbonpart  ${KDE4_KDECORE_LIBS} karboncommon karbonui )

install(TARGETS karbonpart  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

set(karbon_KDEINIT_SRCS main.cc )

kde4_add_kdeinit_executable( karbon ${karbon_KDEINIT_SRCS})

target_link_libraries(kdeinit_karbon  komain ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS}  karboncommon karbonui )

install(TARGETS kdeinit_karbon  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( karbon kdeinit_karbon )
install(TARGETS karbon  ${INSTALL_TARGETS_DEFAULT_ARGS})
