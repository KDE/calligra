########### next target ###############

set(exportepub2_PART_SRCS
    exportepub2.cpp
    htmlconvert.cpp

    libepub/EpubFile.cpp
)


kde4_add_plugin(exportepub2 ${exportepub2_PART_SRCS})


target_link_libraries(exportepub2 komain)

install(TARGETS exportepub2  DESTINATION ${PLUGIN_INSTALL_DIR})


 ########### install files ###############

install( FILES  words_epub2_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
