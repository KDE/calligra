project(wordsapp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../part
    ${CMAKE_CURRENT_BINARY_DIR}/../part
)

set(words_KDEINIT_SRCS main.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/*-apps-calligrawords.png")
ecm_add_app_icon(words_KDEINIT_SRCS ICONS ${ICONS_SRCS})

kf5_add_kdeinit_executable( calligrawords ${words_KDEINIT_SRCS})

if (APPLE)
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.words")
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Words")
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/words_KDEINIT_SRCS.icns DESTINATION ${BUNDLE_INSTALL_DIR}/calligrawords.app/Contents/Resources)
endif ()

target_link_libraries(kdeinit_calligrawords   komain )

install(TARGETS kdeinit_calligrawords  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( calligrawords kdeinit_calligrawords )
install(TARGETS calligrawords  ${INSTALL_TARGETS_DEFAULT_ARGS})

install( PROGRAMS words.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES words.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)
