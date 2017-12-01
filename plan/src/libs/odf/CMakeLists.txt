if(BUILD_TESTING)
    add_subdirectory( tests )
endif()
add_subdirectory( writeodf )

include_directories( ${PLANODF_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}/writeodf )

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

add_library(planodf SHARED ${koodf_LIB_SRCS})
add_dependencies(planodf plan_writeodf.h-target)
generate_export_header(planodf BASE_NAME koodf)

target_link_libraries(planodf
    PUBLIC
        planstore
        Qt5::PrintSupport
        Qt5::Xml
    PRIVATE
        KF5::I18n
)

set_target_properties(planodf PROPERTIES
    VERSION ${GENERIC_PLAN_LIB_VERSION} SOVERSION ${GENERIC_PLAN_LIB_SOVERSION}
)
install(TARGETS planodf ${INSTALL_TARGETS_DEFAULT_ARGS} )

