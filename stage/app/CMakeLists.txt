project(stageapp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../part
    ${CMAKE_CURRENT_BINARY_DIR}/../part
    ${KOMAIN_INCLUDES}
    ${KOPAGEAPP_INCLUDES}
)

set(stage_KDEINIT_SRCS main.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/*-apps-calligrastage.png")
ecm_add_app_icon(stage_KDEINIT_SRCS ICONS ${ICONS_SRCS})

add_executable(calligrastage ${stage_KDEINIT_SRCS})

if (APPLE)
   set_target_properties(calligrastage PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligrastage PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.calligrastage")
   set_target_properties(calligrastage PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Calligra Stage 2")
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/stage_KDEINIT_SRCS.icns DESTINATION ${KDE_INSTALL_BUNDLEDIR}/calligrastage.app/Contents/Resources)
endif ()

target_link_libraries(calligrastage komain)

install(TARGETS calligrastage ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.calligra.stage.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.calligra.stage.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.calligra.stage.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
