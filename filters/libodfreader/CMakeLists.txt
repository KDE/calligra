include_directories(
    ${KOODF_INCLUDES}
    ${KOMAIN_INCLUDES}  # For KoFilter
 )

########### libodfreader ###############

set(odfreader_LIB_SRCS
    #OdtReader.cpp
    #OdtReaderBackend.cpp
    OdfReaderContext.cpp
    OdfParser.cpp
)

kde4_add_library(odfreader SHARED ${odfreader_LIB_SRCS})

target_link_libraries(odfreader koodf ${QT_QTXML_LIBRARY})

set_target_properties(odfreader PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION}
    SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS odfreader ${INSTALL_TARGETS_DEFAULT_ARGS} )


#install( FILES
         # odfreader_export.h
         # OdfReaderContext.h
         # OdtReaderBackend.h
         # OdfTextReaderBackend.h
         # OdtReader.h
         # OdfTextReader.h
         # KoTableTemplate.h
#DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

