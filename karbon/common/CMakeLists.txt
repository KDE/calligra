set(karboncommon_LIB_SRCS 
    KarbonDocument.cpp
    vglobal.cc 
    vcursor.cc

    commands/KarbonLayerReorderCommand.cpp
    commands/KarbonBooleanCommand.cpp
)

kde4_add_library( karboncommon SHARED ${karboncommon_LIB_SRCS} )

target_link_libraries( karboncommon ${KDE4_KDECORE_LIBS} komain kotext )

set_target_properties(karboncommon PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS karboncommon  DESTINATION ${LIB_INSTALL_DIR})
