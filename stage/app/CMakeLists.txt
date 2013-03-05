project(stageapp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../part
    ${KOMAIN_INCLUDES}
    ${KOPAGEAPP_INCLUDES}
)

set(stage_KDEINIT_SRCS main.cpp )

kde4_add_app_icon(stage_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/hi*-app-calligrastage.png")

kde4_add_kdeinit_executable(calligrastage ${stage_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(calligrastage PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligrastage PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.stage")
   set_target_properties(calligrastage PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Calligra Stage 2")
endif (Q_WS_MAC)

target_link_libraries(kdeinit_calligrastage komain )

install(TARGETS kdeinit_calligrastage ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(calligrastage kdeinit_calligrastage)
install(TARGETS calligrastage ${INSTALL_TARGETS_DEFAULT_ARGS})

install( PROGRAMS stage.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
