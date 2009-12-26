add_subdirectory(wv2)
INCLUDE_DIRECTORIES( ${GLIB2_INCLUDE_DIR} ${LIBGSF_INCLUDE_DIR} ${ICONV_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} )

set(mswordodf_import_PART_SRCS
   mswordodfimport.cpp
   document.cpp
   conversion.cpp
   texthandler.cpp
   paragraph.cpp
   tablehandler.cpp
   graphicshandler.cpp )

kde4_add_plugin(mswordodf_import WITH_PREFIX ${mswordodf_import_PART_SRCS})

target_link_libraries(mswordodf_import wv2  komain)

install(TARGETS mswordodf_import  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  kword_msword-odf_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

