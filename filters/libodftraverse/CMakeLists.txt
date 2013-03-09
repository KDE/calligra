include_directories( ${KOODF_INCLUDES} )

########### libodftraverse ###############

set(odftraverse_LIB_SRCS
    OdfParser.cpp
    OdtTraverser.cpp
    OdtTraverserBackend.cpp
)

kde4_add_library(odftraverse SHARED ${odftraverse_LIB_SRCS})

target_link_libraries(odftraverse koodf ${QT_QTXML_LIBRARY})

set_target_properties(odftraverse PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION}
    SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS odftraverse ${INSTALL_TARGETS_DEFAULT_ARGS} )


#install( FILES
         # koodf_export.h
         # KoOdf.h
         # KoTableTemplate.h
#DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

