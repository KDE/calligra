


include_directories( ${KDE4_INCLUDES} )

########### next target ###############

set(thesaurustool_PART_SRCS main.cc )

kde4_automoc(${thesaurustool_PART_SRCS})

kde4_add_plugin(thesaurustool WITH_PREFIX ${thesaurustool_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} thesaurustool )

target_link_libraries(thesaurustool  ${KDE4_KIO_LIBS} )

install(TARGETS thesaurustool  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  thesaurustool.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  thesaurus.txt  DESTINATION ${DATA_INSTALL_DIR}/thesaurus)

