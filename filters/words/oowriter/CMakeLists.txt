

include_directories(${CMAKE_SOURCE_DIR}/filters/liboofilter/)

set(libcommon_SRCS conversion.cc)


add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

########### next target ###############

set(oowriterimport_PART_SRCS oowriterimport.cc ${libcommon_SRCS} ${liboofilter_SRCS})

kde4_add_plugin(oowriterimport ${oowriterimport_PART_SRCS})



target_link_libraries(oowriterimport wordsexportfilters komain ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS oowriterimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(oowriterexport_PART_SRCS oowriterexport.cc ExportFilter.cc ${libcommon_SRCS} ${liboofilter_SRCS})

kde4_add_plugin(oowriterexport ${oowriterexport_PART_SRCS})



target_link_libraries(oowriterexport wordsexportfilters)

install(TARGETS oowriterexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############


########### install files ###############

install( FILES  words_oowriter_import.desktop words_oowriter_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



