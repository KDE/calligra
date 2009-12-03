add_definitions(-DKDE_DEFAULT_DEBUG_AREA=30529)

include_directories( ${KOMAIN_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIRECTORY})

########### next target ###############

set(msooxml_LIB_SRCS
    MsooXmlUtils.cpp
    MsooXmlReader.cpp
    MsooXmlCommonReader.cpp
    MsooXmlThemesReader.cpp
    MsooXmlRelationshipsReader.cpp
    MsooXmlRelationships.cpp
    MsooXmlImport.cpp

    KoOdfExporter.cpp
)

kde4_add_library(msooxml SHARED ${msooxml_LIB_SRCS})

target_link_libraries(msooxml
    komain
)

set_target_properties(msooxml PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS msooxml ${INSTALL_TARGETS_DEFAULT_ARGS})

if (FALSE) # these headers are private for now
install( FILES
    msooxml_export.h
    MsooXmlContentTypes.h
    MsooXmlReader.h
    MsooXmlReader_p.h
    MsooXmlCommonReaderImpl.h
    MsooXmlCommonReader.h
    MsooXmlCommonReaderMethods.h
    MsooXmlSchemas.h
    MsooXmlThemesReader.h
    MsooXmlUtils.h
    MsooXmlRelationships.h
    MsooXmlImport.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/msooxml COMPONENT Devel
)
endif (FALSE)
