include_directories(
    ${KOODF_INCLUDES}
    ${KOODF2_INCLUDES}
    ${KOMAIN_INCLUDES}  # For KoFilter
 )

########### libodfreader ###############

set(odfreader_LIB_SRCS
    OdfReaderDebug.cpp
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
generate_export_header(koodfreader)

target_link_libraries(koodfreader
    PUBLIC
        koodf2
    PRIVATE
        KF6::I18n
)
set_target_properties(koodfreader PROPERTIES
    VERSION ${CALLIGRA_VERSION}
    SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS koodfreader ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


if (SHOULD_BUILD_DEVEL_HEADERS)

#install( FILES
     # ${CMAKE_CURRENT_BINARY_DIR}/koodfreader_export.h
     # OdfReaderContext.h
     # OdtReaderBackend.h
     # OdfParser.h
     # OdtReader.h
     # OdtReaderBackend.h
     # OdfTextReader.h
     # OdfTextReaderBackend.h
     # KoTableTemplate.h
#DESTINATION ${KDE_INSTALL_INCLUDEDIR} COMPONENT Devel)

endif()
