

include_directories(${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR})


########### next target ###############

set(xsltimport_PART_SRCS xsltimport.cc xsltimportdia.cc xsltproc.cc )

kde4_automoc(${xsltimport_PART_SRCS})

kde4_add_ui3_files(xsltimport_PART_SRCS xsltdialog.ui )

kde4_add_plugin(xsltimport WITH_PREFIX ${xsltimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} xsltimport )

target_link_libraries(xsltimport  ${KDE4_KIO_LIBS} kofficecore ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} )

install(TARGETS xsltimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  xslt_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



