include_directories(
    ${KOODF_INCLUDES}
    ${KOODF2_INCLUDES}
    ${KOMAIN_INCLUDES}  # For KoFilter
 )

########### libodfreader ###############

set(odfreader_LIB_SRCS
    OdfReader.cpp
    OdfReaderContext.cpp
    OdfReaderBackend.cpp
    OdfParser.cpp
    OdtReader.cpp
    OdtReaderBackend.cpp
    OdsReader.cpp
    OdsReaderBackend.cpp
    #OdpReader.cpp
    #OdpReaderBackend.cpp

    OdfTextReader.cpp
    OdfTextReaderBackend.cpp
    OdfDrawReader.cpp
    OdfDrawReaderBackend.cpp
    OdfChartReader.cpp
    OdfChartReaderBackend.cpp
)

add_library(koodfreader SHARED ${odfreader_LIB_SRCS})

target_link_libraries(koodfreader koodf koodf2 KF5::KDELibs4Support Qt5::Xml)

set_target_properties(koodfreader PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION}
    SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS koodfreader ${INSTALL_TARGETS_DEFAULT_ARGS} )


#install( FILES
     # koodfreader_export.h
     # OdfReaderContext.h
     # OdtReaderBackend.h
     # OdfParser.h
     # OdtReader.h
     # OdtReaderBackend.h
     # OdfTextReader.h
     # OdfTextReaderBackend.h
     # KoTableTemplate.h
#DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

