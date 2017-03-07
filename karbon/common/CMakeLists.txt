set(karboncommon_LIB_SRCS
    KarbonGlobal.cpp
    KarbonOutlinePaintingStrategy.cpp

    commands/KarbonBooleanCommand.cpp
)

add_library( karboncommon SHARED ${karboncommon_LIB_SRCS} )
generate_export_header(karboncommon BASE_NAME karboncommon)

target_link_libraries(karboncommon
    PUBLIC
        komain
    PRIVATE
        kowidgets
)

set_target_properties(karboncommon PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS karboncommon  ${INSTALL_TARGETS_DEFAULT_ARGS})
