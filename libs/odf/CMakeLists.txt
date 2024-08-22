if(BUILD_TESTING)
    add_subdirectory( tests )
endif()
add_subdirectory( writeodf )

include_directories( ${KOODF_INCLUDES} )

########### libkoodf ###############

set(koodf_LIB_SRCS
    KoOdf.cpp
    KoOdfManifestEntry.cpp
    KoDocumentInfo.cpp
    KoGenStyle.cpp
    KoGenStyles.cpp
    KoFontFace.cpp
    KoOdfLoadingContext.cpp
    KoOasisSettings.cpp
    KoOdfStylesReader.cpp
    KoOdfNumberStyles.cpp
    KoOdfPaste.cpp
    KoOdfReadStore.cpp
    KoOdfWriteStore.cpp
    KoStyleStack.cpp
    KoOdfGraphicStyles.cpp
    KoGenChange.cpp
    KoGenChanges.cpp
    KoDocumentBase.cpp
    KoEmbeddedDocumentSaver.cpp
    KoEmbeddedDocumentLoader.cpp
    KoBorder.cpp
    Ko3dScene.cpp
    KoShadowStyle.cpp
    KoPageLayout.cpp
    KoPageFormat.cpp
    KoColumns.cpp
    KoUnit.cpp
    KoOdfNotesConfiguration.cpp
    KoOdfBibliographyConfiguration.cpp
    KoOdfNumberDefinition.cpp
    KoOdfLineNumberingConfiguration.cpp
    KoElementReference.cpp
    writeodf/helpers.cpp

    OdfDebug.cpp
)

add_library(koodf SHARED ${koodf_LIB_SRCS})
add_dependencies(koodf writeodf.h-target)
generate_export_header(koodf BASE_NAME koodf)

target_link_libraries(koodf
    PUBLIC
        kostore
        Qt6::PrintSupport
        Qt6::Xml
    PRIVATE
        KF6::I18n
        KF6::ConfigCore
        KF6::CoreAddons
)

set_target_properties(koodf PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS koodf ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/koodf_export.h
    KoOdf.h
    KoOdfManifestEntry.h
    KoGenStyle.h
    KoGenStyles.h
    KoFontFace.h
    KoOdfLoadingContext.h
    KoOasisSettings.h
    KoOdfStylesReader.h
    KoOdfNotesConfiguration.h
    KoOdfBibliographyConfiguration.h
    KoOdfNumberStyles.h
    KoOdfNumberDefinition.h
    KoOdfLineNumberingConfiguration.h
    KoOdfPaste.h
    KoOdfReadStore.h
    KoOdfWriteStore.h
    KoStyleStack.h
    KoOdfGraphicStyles.h
    KoDocumentBase.h
    KoGenChange.h
    KoGenChanges.h
    KoEmbeddedDocumentSaver.h
    KoEmbeddedDocumentLoader.h
    KoBorder.h
    Ko3dScene.h
    KoShadowStyle.h
    KoPageLayout.h
    KoPageFormat.h
    KoColumns.h
    KoUnit.h
    KoElementReference.h
    KoTableTemplate.h
DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel)

endif()
