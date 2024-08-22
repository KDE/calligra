add_definitions(-DTRANSLATION_DOMAIN=\"okularGenerator_odp\")

include_directories( ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES})

## shared odp generator lib

set( kookularGenerator_odp_SRCS OkularOdpGenerator.cpp )

add_library( kookularGenerator_odp SHARED ${kookularGenerator_odp_SRCS})
generate_export_header(kookularGenerator_odp)

target_link_libraries(kookularGenerator_odp
    PUBLIC
        Okular::Core
    PRIVATE
        komain
        kopageapp
)

set_target_properties(kookularGenerator_odp PROPERTIES
   VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)

install(TARGETS kookularGenerator_odp  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


## odp generator plugin

kcoreaddons_add_plugin(okularGenerator_odp_calligra
    INSTALL_NAMESPACE "okular_generators"
    SOURCES OkularOdpGeneratorPlugin.cpp
)

target_link_libraries( okularGenerator_odp_calligra
    kookularGenerator_odp
)

install( PROGRAMS okularApplication_odp_calligra.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
