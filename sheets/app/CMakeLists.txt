
set (calligrasheets_KDEINIT_SRCS Main.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/pics/*-apps-calligrasheets.png")
ecm_add_app_icon(kdeinit_app_ICONS_SRCS ICONS ${ICONS_SRCS})
if(WIN32)
    set(_resourcefile "${CMAKE_CURRENT_BINARY_DIR}/kdeinit_app_ICONS_SRCS.rc")
endif()

kf5_add_kdeinit_executable( calligrasheets ${calligrasheets_KDEINIT_SRCS})

if (APPLE)
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.sheets")
set_target_properties(calligrasheets PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Calligra Sheets 2")
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/calligrasheets_KDEINIT_SRCS.icns DESTINATION ${BUNDLE_INSTALL_DIR}/calligrasheets.app/Contents/Resources)
endif ()

target_link_libraries(kdeinit_calligrasheets calligrasheetsui komain )

target_include_directories( kdeinit_calligrasheets
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_BINARY_DIR}/../
)

install(TARGETS kdeinit_calligrasheets  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( calligrasheets kdeinit_calligrasheets )
install(TARGETS calligrasheets  ${INSTALL_TARGETS_DEFAULT_ARGS})

install( PROGRAMS  org.kde.calligrasheets.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES org.kde.calligrasheets.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})


