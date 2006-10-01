include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/visitors )



########### next target ###############

set(karbon_defaulttools_PART_SRCS 
   vcurvefit.cc 
   vrotatetool.cc 
   vselecttool.cc
   vsheartool.cc 
   vellipsetool.cc 
   vgradienttool.cc 
   vpatterntool.cc 
   vpenciltool.cc 
   vpolygontool.cc 
   vpolylinetool.cc 
   vrectangletool.cc 
   vroundrecttool.cc 
   vshapetool.cc 
   vsinustool.cc 
   vspiraltool.cc 
   vstartool.cc 
   vtexttool.cc 
   vdefaulttools.cc )

kde4_automoc(${karbon_defaulttools_PART_SRCS})

kde4_add_plugin(karbon_defaulttools ${karbon_defaulttools_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_defaulttools )

target_link_libraries(karbon_defaulttools  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kofficeui karboncommon )

install(TARGETS karbon_defaulttools  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  karbondefaulttools.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
