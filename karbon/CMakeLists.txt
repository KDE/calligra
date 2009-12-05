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
    ${FLAKE_INCLUDES}
    ${KOTEXT_INCLUDES}
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/libs/widgets/resources
    ${CMAKE_SOURCE_DIR}/libs/widgets/colorwidgets
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

target_link_libraries(karbonpart karbonui)

install(TARGETS karbonpart  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

set(karbon_KDEINIT_SRCS main.cc )

kde4_add_app_icon( karbon_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-karbon.png" )
kde4_add_kdeinit_executable( karbon ${karbon_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(karbon PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(karbon PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.koffice.karbon")
   set_target_properties(karbon PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Karbon 2")
endif (Q_WS_MAC)

target_link_libraries(kdeinit_karbon karbonui)

install(TARGETS kdeinit_karbon  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(karbon kdeinit_karbon)
install(TARGETS karbon  ${INSTALL_TARGETS_DEFAULT_ARGS})

