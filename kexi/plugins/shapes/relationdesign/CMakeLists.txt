
include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi/main ${CMAKE_SOURCE_DIR}/kexi/kexidb)

set(kexirelationdesignshape_PART_SRCS  
kexirelationdesignshape.cpp
kexirelationdesignfactory.cpp
kexirelationdesignshapeplugin.cpp
kexirelationdesigntool.cpp
kexirelationdesigntoolfactory.cpp
simplefield.cpp
)

kde4_add_plugin(kexirelationdesignshape ${kexirelationdesignshape_PART_SRCS})

target_link_libraries(
kexirelationdesignshape
${CALLIGRADB_LIBS}
kexicore
keximain
komain
${KDE4_KDECORE_LIBS}
${KDE4_KDEUI_LIBS}
${QT_QTCORE_LIBRARY}
${QT_QTGUI_LIBRARY}
${QT_QT3SUPPORT_LIBRARY}
)

install(TARGETS kexirelationdesignshape  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES kexirelationdesignshape.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

