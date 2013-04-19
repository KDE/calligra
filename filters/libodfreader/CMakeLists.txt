include_directories(
    ${KOODF_INCLUDES}
    ${KOODF2_INCLUDES}
    ${KOMAIN_INCLUDES}  # For KoFilter
 )

########### libodfreader ###############

set(odfreader_LIB_SRCS
    OdtReader.cpp
    OdfReaderContext.cpp
    OdtReaderBackend.cpp
    OdfParser.cpp
)

kde4_add_library(koodfreader SHARED ${odfreader_LIB_SRCS})

target_link_libraries(koodfreader koodf koodf2 ${QT_QTXML_LIBRARY})

set_target_properties(koodfreader PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION}
    SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS koodfreader ${INSTALL_TARGETS_DEFAULT_ARGS} )


#install( FILES
         # koodfreader_export.h
         # OdfReaderContext.h
         # OdtReaderBackend.h
         # OdfTextReaderBackend.h
         # OdtReader.h
         # OdfTextReader.h
         # KoTableTemplate.h
#DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

