project(wordsapp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../part
    ${CMAKE_CURRENT_BINARY_DIR}/../part
)

set(words_KDEINIT_SRCS main.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/*-apps-calligrawords.png")
ecm_add_app_icon(kdeinit_app_ICONS_SRCS ICONS ${ICONS_SRCS})
if(WIN32)
    set(_resourcefile "${CMAKE_CURRENT_BINARY_DIR}/kdeinit_app_ICONS_SRCS.rc")
endif()

add_executable( calligrawords ${words_KDEINIT_SRCS})

if (APPLE)
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.calligrawords")
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Words")
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/words_KDEINIT_SRCS.icns DESTINATION ${KDE_INSTALL_BUNDLEDIR}/calligrawords.app/Contents/Resources)
endif ()

target_link_libraries(calligrawords   komain )

install(TARGETS calligrawords  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(TARGETS calligrawords  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.calligra.words.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.calligra.words.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.calligra.words.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
