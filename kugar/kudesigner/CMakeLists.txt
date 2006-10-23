


include_directories(
    ${KOFFICECORE_INCLUDES} ${KOFFICEUI_INCLUDES})


add_subdirectory( toolbar )
add_subdirectory( pics )
add_subdirectory( templates )




########### next target ###############

set(kudesignercore_LIB_SRCS ${libkudesignercommon_SRCS} kudesigner_view.cpp kudesigner_factory.cpp kudesigner_doc.cpp )

kde4_automoc(${kudesignercore_LIB_SRCS})

kde4_add_library(kudesignercore SHARED ${kudesignercore_LIB_SRCS})

target_link_libraries(kudesignercore  kofficeui ${KDE4_KPARTS_LIBS} kofficecore koproperty ${KDE4_KIO_LIBS} ${KDE4_KDEPRINT_LIBS} )

set_target_properties(kudesignercore PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kudesignercore  DESTINATION ${LIB_INSTALL_DIR})


########### next target ###############

set(kudesignerpart_PART_SRCS kudesigner_part.cpp )

kde4_automoc(${kudesignerpart_PART_SRCS})

kde4_add_plugin(kudesignerpart WITH_PREFIX ${kudesignerpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kudesignerpart )

target_link_libraries(kudesignerpart  kudesignercore kofficeui ${KDE4_KPARTS_LIBS} kofficecore )

install(TARGETS kudesignerpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kudesigner_KDEINIT_SRCS main.cpp )

kde4_automoc(${kudesigner_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kudesigner ${kudesigner_KDEINIT_SRCS})

target_link_libraries(kdeinit_kudesigner  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_kudesigner  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kudesigner kdeinit_kudesigner )
install(TARGETS kudesigner  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kudesigner.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES  kudesignerui.rc kudesigner_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kudesigner)




