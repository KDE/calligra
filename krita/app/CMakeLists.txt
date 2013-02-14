set( krita_SRCS
  kis_main.cpp
  kis_program.cpp
  kis_about.cpp
  kis_mainwindow.cpp
)

kde4_add_app_icon(krita "${CMAKE_CURRENT_SOURCE_DIR}/../pics/app/*.png")
kde4_add_executable( krita  ${krita_SRCS} )

target_link_libraries( krita kritaui )

install( TARGETS krita ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( PROGRAMS ../krita.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES ../krita.rc  DESTINATION ${DATA_INSTALL_DIR}/krita )
