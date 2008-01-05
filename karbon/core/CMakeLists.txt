include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOGUIUTILS_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon ${FREETYPE_INCLUDE_DIR} ${FreeType2_INCLUDE_DIR} ${FONTCONFIG_INCLUDE_DIR} )

#add_definitions(${FREETYPE_INCLUDE_DIR})

set(karboncore_SRCS
    KarbonDocument.cpp
    vglobal.cc 
    vcursor.cc
    )


#kde4_add_dcop_skels(karboncore_SRCS
#    vcomposite_iface.h
#    vobject_iface.h
#    vgroup_iface.h
#    vlayer_iface.h 
#    vtext_iface.h
#    )


kde4_add_library(karboncore ${karboncore_SRCS})



target_link_libraries(karboncore ${FREETYPE_LIBRARIES} ${FONTCONFIG_LIBRARIES})

install(TARGETS karboncore  DESTINATION ${LIB_INSTALL_DIR})
