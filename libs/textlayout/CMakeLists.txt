include_directories(${TEXTLAYOUT_INCLUDES})

if(BUILD_TESTING)
    add_subdirectory( tests )
endif()

########### next target ###############
set(textlayout_LIB_SRCS
    KoTextLayoutCellHelper.cpp
    KoTextLayoutArea.cpp
    KoTextLayoutArea_paint.cpp
    KoTextLayoutEndNotesArea.cpp
    KoTextLayoutTableArea.cpp
    KoTextLayoutNoteArea.cpp
    KoTextLayoutRootArea.cpp
    KoTextLayoutRootAreaProvider.cpp
    KoTextDocumentLayout.cpp
    ListItemsHelper.cpp
    KoTextShapeContainerModel.cpp
    RunAroundHelper.cpp
    KoTextLayoutObstruction.cpp
    FrameIterator.cpp
    TableIterator.cpp
    KoPointedAt.cpp
    KoTextShapeData.cpp
    FloatingAnchorStrategy.cpp
    InlineAnchorStrategy.cpp
    AnchorStrategy.cpp
    ToCGenerator.cpp
    DummyDocumentLayout.cpp
    IndexGeneratorManager.cpp
    KoStyleThumbnailer.cpp
    TextLayoutDebug.cpp
)


add_library(kotextlayout SHARED ${textlayout_LIB_SRCS})
generate_export_header(kotextlayout BASE_NAME kotextlayout)

target_link_libraries(kotextlayout
    PUBLIC
        kotext
)

set_target_properties(kotextlayout PROPERTIES
   VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS kotextlayout  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if (SHOULD_BUILD_DEVEL_HEADERS)

install(
    FILES
    KoTextDocumentLayout.h
    KoTextLayoutArea.h
    KoTextLayoutRootArea.h
    KoTextShapeData.h
    KoPointedAt.h
    KoStyleThumbnailer.h
    DESTINATION
    ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel
)

endif()
