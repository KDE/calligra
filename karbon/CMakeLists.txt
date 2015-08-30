project(karbon)

#Set the correct compiler options
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
   # 32 bit
   message(STATUS "Karbon detected that you use a 32 bit processor.")
else()
   # 64 bit (well, anything else than 32 bit, but someone use something else than 32 or 64 bit ?)
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
   message(STATUS "Karbon detected that you use a 64 bit processor. Added -fPIC to the CXX_FLAGS.")
endif()

add_definitions(-DTRANSLATION_DOMAIN=\"karbon\")

include_directories(
    ${FLAKE_INCLUDES}
    ${KOTEXT_INCLUDES}
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/libs/widgets/resources
    ${CMAKE_SOURCE_DIR}/karbon/ui
    ${CMAKE_SOURCE_DIR}/karbon/common
    ${CMAKE_SOURCE_DIR}/karbon/common/commands
)

add_subdirectory( common )
add_subdirectory( ui )
add_subdirectory( plugins )
add_subdirectory( stencils )
add_subdirectory( data )
add_subdirectory( templates )

########### next target ###############

set(karbonpart_PART_SRCS KarbonFactoryInit.cpp )

add_library(karbonpart MODULE ${karbonpart_PART_SRCS})
kcoreaddons_desktop_to_json(karbonpart "${CMAKE_CURRENT_SOURCE_DIR}/data/karbonpart.desktop")

target_link_libraries(karbonpart karbonui)

install(TARGETS karbonpart  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

########### next target ###############

if(NOT RELEASE_BUILD)
    add_definitions(-DMAINTANER_WANTED_SPLASH)
endif()

set(karbon_KDEINIT_SRCS main.cpp )

file(GLOB karbon_icons "${CMAKE_CURRENT_SOURCE_DIR}/pics/action/*-actions-*.png")
ecm_install_icons(ICONS ${karbon_icons} DESTINATION ${KDE_INSTALL_DATADIR}/karbon/icons)

file(GLOB karbon_app_icons "${CMAKE_CURRENT_SOURCE_DIR}/pics/app/*-apps-calligrakarbon.png")
ecm_add_app_icon(karbon_KDEINIT_SRCS ICONS ${karbon_app_icons})
ecm_install_icons(ICONS ${karbon_app_icons} DESTINATION ${KDE_INSTALL_ICONDIR})

kf5_add_kdeinit_executable( karbon ${karbon_KDEINIT_SRCS})

if (Q_OS_MAC)
   set_target_properties(karbon PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(karbon PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.karbon")
   set_target_properties(karbon PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Karbon")
endif()

target_link_libraries(kdeinit_karbon karbonui)

install(TARGETS kdeinit_karbon  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(karbon kdeinit_karbon)
install(TARGETS karbon  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

