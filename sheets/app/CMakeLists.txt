
set (calligrasheets_KDEINIT_SRCS Main.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/pics/*-apps-calligrasheets.png")
ecm_add_app_icon(kdeinit_app_ICONS_SRCS ICONS ${ICONS_SRCS})
if(WIN32)
    set(_resourcefile "${CMAKE_CURRENT_BINARY_DIR}/kdeinit_app_ICONS_SRCS.rc")
endif()

add_executable( calligrasheets ${calligrasheets_KDEINIT_SRCS})

if (APPLE)
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.sheets")
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Calligra Sheets 2")
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/calligrasheets_KDEINIT_SRCS.icns DESTINATION ${KDE_INSTALL_BUNDLEDIR}/calligrasheets.app/Contents/Resources)
endif ()

target_link_libraries(calligrasheets calligrasheetsui komain )

target_include_directories(calligrasheets
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_BINARY_DIR}/../
)

install(TARGETS calligrasheets  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.calligra.sheets.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.calligra.sheets.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.calligra.sheets.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)


