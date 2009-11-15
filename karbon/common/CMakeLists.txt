set(karboncommon_LIB_SRCS 
    KarbonDocument.cpp
    KarbonGlobal.cpp
    KarbonGradientHelper.cpp
    KarbonCursor.cpp
    KarbonOutlinePaintingStrategy.cpp

    commands/KarbonLayerReorderCommand.cpp
    commands/KarbonBooleanCommand.cpp
)

kde4_add_library( karboncommon SHARED ${karboncommon_LIB_SRCS} )

target_link_libraries(karboncommon komain kowidgets)
target_link_libraries(karboncommon LINK_INTERFACE_LIBRARIES komain)

set_target_properties(karboncommon PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS karboncommon  ${INSTALL_TARGETS_DEFAULT_ARGS})
