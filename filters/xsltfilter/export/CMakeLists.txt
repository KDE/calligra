

add_subdirectory( xsl )

include_directories(${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR})



########### next target ###############

set(xsltexport_PART_SRCS xsltexport.cc xsltexportdia.cc xsltproc.cc )

kde4_automoc(${xsltexport_PART_SRCS})

kde4_add_ui3_files(xsltexport_PART_SRCS xsltdialog.ui )

kde4_add_plugin(xsltexport WITH_PREFIX ${xsltexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} xsltexport )

target_link_libraries(xsltexport  ${KDE4_KIO_LIBS} kofficecore ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} )

install(TARGETS xsltexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  xslt_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


