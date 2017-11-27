if (BUILD_TESTING)
    add_subdirectory( tests )
endif ()

kde_enable_exceptions()

if (APPLE)
    find_library(FOUNDATION_LIBRARY Foundation)
endif ()

include_directories( ${PLANMAIN_INCLUDES})

########### next target ###############

if( Qca-qt5_FOUND )
   add_definitions( -DQCA2 )
endif()

set(komain_LIB_SRCS
    KoAutoSaveRecoveryDialog.cpp
    KoApplication.cpp
    KoComponentData.cpp
    KoDockFactoryBase.cpp
    KoDockRegistry.cpp
    KoDocument.cpp
    KoDocumentEntry.cpp
    KoMainWindow.cpp
    KoPrintingDialog.cpp
    KoPrintJob.cpp
    KoView.cpp
    KoFilterManager.cpp
    KoFilterChain.cpp
    KoFilter.cpp
    KoFilterEntry.cpp
    KoFilterManager_p.cpp
    KoFilterVertex.cpp
    KoFilterGraph.cpp
    KoFilterEdge.cpp
    KoFilterChainLink.cpp
    KoFilterChainLinkList.cpp
    KoUndoStackAction.cpp

#     KoFindBase.cpp
#     KoFindMatch.cpp
#     KoFindOption.cpp
#     KoFindOptionSet.cpp

    KoPart.cpp

    MainDebug.cpp
    Calligra2Migration.cpp
)

if( Qt5DBus_FOUND )
    set(komain_LIB_SRCS ${komain_LIB_SRCS}
        KoApplicationAdaptor.cpp
        KoViewAdaptor.cpp
        KoPartAdaptor.cpp
    )
endif()

add_library(planmain SHARED  ${komain_LIB_SRCS})
generate_export_header(planmain
    EXPORT_FILE_NAME komain_generated_export.h BASE_NAME komain
)

target_link_libraries(planmain
    PUBLIC
        planwidgets
        plankundo2
    PRIVATE
        planplugin
        KF5::Notifications
        KF5::ItemViews
        KF5::KIOFileWidgets
        KF5::IconThemes
        KF5::DBusAddons
)

if( KF5Activities_FOUND )
    target_link_libraries(planmain PRIVATE KF5::Activities)
endif()

if (APPLE)
    target_link_libraries(planmain PRIVATE ${FOUNDATION_LIBRARY})
endif ()

set_target_properties(planmain PROPERTIES
    VERSION ${GENERIC_PLAN_LIB_VERSION} SOVERSION ${GENERIC_PLAN_LIB_SOVERSION}
)
install(TARGETS planmain ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# calligradocker.desktop

install(FILES calligraplan_shell.rc DESTINATION ${DATA_INSTALL_DIR}/calligraplan )

if (APPLE)
    install(FILES osx.stylesheet DESTINATION ${DATA_INSTALL_DIR}/calligraplan )
endif ()

