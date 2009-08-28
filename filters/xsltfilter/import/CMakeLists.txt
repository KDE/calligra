
include_directories(${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/..)

########### next target ###############

set(xsltimport_PART_SRCS xsltimport.cc xsltimportdia.cc xsltproc.cc )


kde4_add_ui3_files(xsltimport_PART_SRCS xsltdialog.ui )

kde4_add_plugin(xsltimport WITH_PREFIX ${xsltimport_PART_SRCS})



target_link_libraries(xsltimport komain ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS xsltimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  xslt_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



