

add_subdirectory( xsl )

include_directories(${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/..)



########### next target ###############

set(xsltexport_PART_SRCS xsltexport.cc xsltexportdia.cc xsltproc.cc )

kde4_add_ui3_files(xsltexport_PART_SRCS xsltdialog.ui )

kde4_add_plugin(xsltexport WITH_PREFIX ${xsltexport_PART_SRCS})



target_link_libraries(xsltexport komain ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS xsltexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  xslt_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


