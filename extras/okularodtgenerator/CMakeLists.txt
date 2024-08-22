add_definitions(-DTRANSLATION_DOMAIN=\"okularGenerator_odt\")

include_directories( ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/words/part)

## shared odt generator lib

set( kookularGenerator_odt_SRCS OkularOdtGenerator.cpp )

add_library( kookularGenerator_odt SHARED ${kookularGenerator_odt_SRCS})
generate_export_header(kookularGenerator_odt)

target_link_libraries(kookularGenerator_odt
    PUBLIC
        Okular::Core
    PRIVATE
        komain
        kotextlayout
        wordsprivate
)

set_target_properties(kookularGenerator_odt PROPERTIES
   VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)

install(TARGETS kookularGenerator_odt  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


## odt generator plugin

kcoreaddons_add_plugin(okularGenerator_odt_calligra
    INSTALL_NAMESPACE "okular_generators"
    SOURCES OkularOdtGeneratorPlugin.cpp
)

target_link_libraries( okularGenerator_odt_calligra
    kookularGenerator_odt
)

install( PROGRAMS okularApplication_odt_calligra.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
