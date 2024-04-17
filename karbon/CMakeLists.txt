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
    ${KOPAGEAPP_INCLUDES}
    ${FLAKE_INCLUDES}
    ${KOTEXT_INCLUDES}
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/karbon/ui
    ${CMAKE_SOURCE_DIR}/karbon/common
    ${CMAKE_SOURCE_DIR}/karbon/common/commands
)

add_subdirectory( common )
add_subdirectory( ui )
add_subdirectory( plugins )
add_subdirectory( data )
add_subdirectory( templates )

########### next target ###############

set(karbonpart_PART_SRCS KarbonFactoryInit.cpp )

add_library(karbonpart MODULE ${karbonpart_PART_SRCS})

target_link_libraries(karbonpart karbonui)

install(TARGETS karbonpart  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/parts)

########### next target ###############

if(NOT RELEASE_BUILD)
    #add_definitions(-DMAINTANER_WANTED_SPLASH)
endif()

set(karbon_KDEINIT_SRCS main.cpp )

file(GLOB karbon_icons "${CMAKE_CURRENT_SOURCE_DIR}/pics/action/*-actions-*.png")
ecm_install_icons(ICONS ${karbon_icons} DESTINATION ${KDE_INSTALL_DATADIR}/karbon/icons)

file(GLOB karbon_app_icons "${CMAKE_CURRENT_SOURCE_DIR}/pics/app/*-apps-calligrakarbon.png")
ecm_add_app_icon(kdeinit_app_ICONS_SRCS ICONS ${karbon_app_icons})
if(WIN32)
    set(_resourcefile "${CMAKE_CURRENT_BINARY_DIR}/kdeinit_app_ICONS_SRCS.rc")
endif()
ecm_install_icons(ICONS
    ${karbon_app_icons}
    "${CMAKE_CURRENT_SOURCE_DIR}/pics/app/sc-apps-calligrakarbon.svgz"
    DESTINATION ${KDE_INSTALL_ICONDIR}
)

add_executable( karbon ${karbon_KDEINIT_SRCS})

if (APPLE)
   set_target_properties(karbon PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(karbon PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.karbon")
   set_target_properties(karbon PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Karbon")
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/karbon_KDEINIT_SRCS.icns DESTINATION ${KDE_INSTALL_BUNDLEDIR}/karbon.app/Contents/Resources)
endif()

target_link_libraries(karbon karbonui)

install(TARGETS karbon  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

