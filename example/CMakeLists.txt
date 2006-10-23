

include_directories( ${KOFFICEUI_INCLUDES} )


########### next target: the part ###############

set(koexamplepart_PART_SRCS KoExamplePart.cpp KoExampleView.cpp KoExampleFactory.cpp )

kde4_automoc(${koexamplepart_PART_SRCS})

kde4_add_plugin(koexamplepart WITH_PREFIX ${koexamplepart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} koexamplepart )

target_link_libraries(koexamplepart  ${KDE4_KPARTS_LIBS} kofficecore )

install(TARGETS koexamplepart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target: the executable ###############

set(koexample_KDEINIT_SRCS main.cpp )

kde4_automoc(${koexample_KDEINIT_SRCS})

kde4_add_kdeinit_executable( koexample ${koexample_KDEINIT_SRCS})

target_link_libraries(kdeinit_koexample  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_koexample  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( koexample kdeinit_koexample )

install(TARGETS koexample  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

## Note: for your application, the mime type should be defined in
##       kdelibs CVS HEAD (and freedesktop.org) and a compatibility entry
##       should be added in the directory koffice/mimetypes/kde??
##       where ?? is the version number of the last stable kde(libs) release.
install( FILES  x-vnd.kde.koexample.desktop  DESTINATION ${MIME_INSTALL_DIR}/application)
install( FILES  examplepart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  example.rc example_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/example)
install( FILES  example.desktop  DESTINATION ${XDG_APPS_DIR})
