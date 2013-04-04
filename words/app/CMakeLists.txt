project(wordsapp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../part
)

set(words_KDEINIT_SRCS main.cpp )

kde4_add_app_icon(words_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/hi*-app-calligrawords.png")

kde4_add_kdeinit_executable( calligrawords ${words_KDEINIT_SRCS})

if (Q_OS_MAC)
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.words")
   set_target_properties(calligrawords PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Words")
endif (Q_OS_MAC)

target_link_libraries(kdeinit_calligrawords  ${KDE4_KDEUI_LIBS} komain )

install(TARGETS kdeinit_calligrawords  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( calligrawords kdeinit_calligrawords )
install(TARGETS calligrawords  ${INSTALL_TARGETS_DEFAULT_ARGS})

install( PROGRAMS words.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
