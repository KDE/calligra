
########### next target ###############

#set(karbonepsexport_PART_SRCS epsexport.cc epsexportdlg.cc )

#kde4_add_plugin(karbonepsexport WITH_PREFIX ${karbonepsexport_PART_SRCS})



#target_link_libraries(karbonepsexport  ${KDE4_KDECORE_LIBS}  karboncommon )

#install(TARGETS karbonepsexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

macro_optional_find_package(Pstoedit)
macro_log_feature(PSTOEDIT_SVG_PLUGIN_FOUND "pstoedit with svg plugin" "Translates PostScript and PDF graphics to SVG" "http://www.pstoedit.net/" FALSE "" "Required at runtime by the Karbon EPS import filter")

if (PSTOEDIT_SVG_PLUGIN_FOUND)

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

set(karbonepsimport_PART_SRCS epsimport.cc pscommentlexer.cc )

kde4_add_plugin(karbonepsimport WITH_PREFIX ${karbonepsimport_PART_SRCS})

target_link_libraries(karbonepsimport komain)

install(TARGETS karbonepsimport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(
FILES  
#karbon_eps_export.desktop  
	karbon_eps_import.desktop
#  	karbon_ps_import.desktop
DESTINATION ${SERVICES_INSTALL_DIR})

endif (PSTOEDIT_SVG_PLUGIN_FOUND)

