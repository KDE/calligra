add_subdirectory(conversion)
INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} ../../libmso )

set(htmlodf_export_PART_SRCS
htmlodfexport.cpp
document.cpp
exportdialog.cpp
convert.cpp
)

kde4_add_ui_files(htmlodf_export_PART_SRCS exportwidget.ui )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

kde4_add_plugin(htmlodf_export WITH_PREFIX ${htmlodf_export_PART_SRCS})

target_link_libraries(
    htmlodf_export
    kwordexportfilters
    koodf
    komain
    QtXmlPatterns
    ${ZLIB_LIBRARIES}
)

install(TARGETS htmlodf_export DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES 
  html-odf_export.desktop
  DESTINATION ${SERVICES_INSTALL_DIR}
)
