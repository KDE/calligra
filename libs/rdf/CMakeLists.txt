add_subdirectory( tests )

kde_enable_exceptions()

include_directories( ${KORDF_INCLUDES})

########### next target ###############

set(kordf_LIB_SRCS
    KoDocumentRdf.cpp
    KoDocumentRdfEditWidget.cpp
    KoSopranoTableModelDelegate.cpp
    KoSopranoTableModel.cpp
    KoRdfPrefixMapping.cpp
    KoSemanticStylesheet.cpp
    KoRdfBasicSemanticItem.cpp
    KoRdfSemanticItem.cpp
    KoRdfSemanticItemFactoryBase.cpp
    KoRdfSemanticItemRegistry.cpp
    KoRdfSemanticItemViewSite.cpp
    KoRdfSemanticTree.cpp
    KoRdfSemanticTreeWidgetItem.cpp
    KoSemanticStylesheetsEditor.cpp
    KoChangeTrackerDisabledRAII.cpp
    RdfSemanticTreeWidgetAction.cpp
    RdfSemanticTreeWidgetSelectAction.cpp
    InsertSemanticObjectActionBase.cpp
    InsertSemanticObjectCreateAction.cpp
    InsertSemanticObjectReferenceAction.cpp
)

ki18n_wrap_ui( kordf_LIB_SRCS
   KoDocumentRdfEditWidget.ui
   KoSemanticStylesheetsEditor.ui
)

add_library(kordf SHARED ${kordf_LIB_SRCS})

target_link_libraries(kordf
    PUBLIC
        komain
        ${SOPRANO_LIBRARIES}
    PRIVATE
        koplugin
)

set_target_properties(kordf PROPERTIES
VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS kordf ${INSTALL_TARGETS_DEFAULT_ARGS} )

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# calligra_semanticitem.desktop

if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    kordf_export.h
    RdfForward.h
    KoDocumentRdf.h
    KoDocumentRdfEditWidget.h
    KoRdfPrefixMapping.h
    KoRdfBasicSemanticItem.h
    KoRdfSemanticItem.h
    KoRdfSemanticItemFactoryBase.h
    KoRdfSemanticItemRegistry.h
    KoRdfSemanticItemViewSite.h
    KoRdfSemanticTree.h
    KoRdfSemanticTreeWidgetItem.h
    KoSemanticStylesheet.h
    KoSemanticStylesheetsEditor.h
    RdfSemanticTreeWidgetAction.h
    RdfSemanticTreeWidgetSelectAction.h
DESTINATION ${INCLUDE_INSTALL_DIR}/calligra COMPONENT Devel)

endif()
