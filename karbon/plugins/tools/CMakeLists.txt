include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

########### next target ###############

set(karbon_tools_PART_SRCS
   KarbonToolsPlugin.cpp
   KarbonCurveFit.cpp
   KarbonPencilTool.cpp
   KarbonPencilToolFactory.cpp )

kde4_automoc(${karbon_tools_PART_SRCS})

kde4_add_plugin(karbontools ${karbon_tools_PART_SRCS})

target_link_libraries(karbontools ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kofficeui )

install(TARGETS karbontools DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES karbontools.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
