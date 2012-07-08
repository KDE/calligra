add_subdirectory(rtf-qt)

include_directories(
    ./rtf-qt/src/
)

set(rtfimport_PART_SRCS rtfimport.cpp )
kde4_add_plugin(rtfimport ${rtfimport_PART_SRCS})
target_link_libraries(rtfimport RtfReader komain)

install(TARGETS rtfimport  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  words_rtf_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  words_rtf_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
