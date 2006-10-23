include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/visitors ${FREETYPE_INCLUDE_DIR} ${FreeType2_INCLUDE_DIR} ${FONTCONFIG_INCLUDE_DIR} )

add_definitions(${FREETYPE_INCLUDE_DIR})

set(karboncore_SRCS
    vcolor.cc 
    vcomposite.cc 
#    vcomposite_iface.cc  
    vdashpattern.cc 
    vdocument.cc 
    vfill.cc 
    vglobal.cc 
    vgradient.cc 
    vgroup.cc 
#    vobject_iface.cc 
#    vgroup_iface.cc 
    vimage.cc 
    vkarbonplugin.cc 
    vlayer.cc 
    vobject.cc 
    vpath.cc 
    vpattern.cc 
    vsegment.cc 
    vselection.cc 
    vstroke.cc 
    vvisitor.cc 
#    vlayer_iface.cc 
    vtext.cc 
#    vtext_iface.cc  
    vclipgroup.cc 
    vcursor.cc
    )


#kde4_add_dcop_skels(karboncore_SRCS
#    vcomposite_iface.h
#    vobject_iface.h
#    vgroup_iface.h
#    vlayer_iface.h 
#    vtext_iface.h
#    )

kde4_automoc(${karboncore_SRCS})

kde4_add_library(karboncore ${karboncore_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karboncore )

target_link_libraries(karboncore ${FREETYPE_LIBRARIES} ${FONTCONFIG_LIBRARIES})

install(TARGETS karboncore  DESTINATION ${LIB_INSTALL_DIR})
