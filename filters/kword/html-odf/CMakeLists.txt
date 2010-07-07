INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} ../../libmso )

set(htmlodf_export_PART_SRCS
htmlodfexport.cpp
document.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

kde4_add_plugin(htmlodf_export WITH_PREFIX ${htmlodf_export_PART_SRCS})

target_link_libraries(
    htmlodf_export
    koodf
    komain
    ${ZLIB_LIBRARIES}
)

install(TARGETS htmlodf_export DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES 
  html-odf_export.desktop
  DESTINATION ${SERVICES_INSTALL_DIR}
)
