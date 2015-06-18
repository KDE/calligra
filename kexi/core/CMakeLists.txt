add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44020)

include_directories( ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/kexi   )

########### next target ###############

set(kexicore_LIB_SRCS
   kexi_global.cpp
   kexi.cpp
   kexiaboutdata.cpp
   KexiMainWindowIface.cpp
   KexiStandardAction.cpp
   kexidbconnectionset.cpp
   kexiprojectset.cpp
   kexiactionproxy.cpp
   kexisharedactionhost.cpp
   kexiactioncategories.cpp
   kexiproject.cpp
   KexiWindow.cpp
   KexiWindowData.cpp
   KexiView.cpp
   kexipartmanager.cpp
   kexipartinfo.cpp
   kexipartitem.cpp
   kexipartbase.cpp
   kexipart.cpp
   kexipartguiclient.cpp
   kexiprojectdata.cpp
   KexiRecentProjects.cpp
   kexiinternalpart.cpp
   #TODO KEXI3 kexidragobjects.cpp
   kexistartupdata.cpp
   KexiCommandLineOptions.cpp
   kexiguimsghandler.cpp
   kexitextmsghandler.cpp
   kexidataiteminterface.cpp
   kexidbshortcutfile.cpp
   kexiblobbuffer.cpp
   #TODO KEXI3 kexistaticpart.cpp
   kexitabledesignerinterface.cpp
   kexisearchandreplaceiface.cpp
   kexitemplateloader.cpp
   KexiRecordNavigatorHandler.cpp
   KexiRecordNavigatorIface.cpp
   KexiSearchableModel.cpp
)

add_library(kexicore SHARED ${kexicore_LIB_SRCS})

generate_export_header(kexicore)

target_link_libraries(kexicore
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets

        KF5::KIOWidgets
        KF5::CoreAddons

        kexiutils
        kowidgetutils

        KDb
        KProperty
)

set_target_properties(kexicore PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS kexicore  ${INSTALL_TARGETS_DEFAULT_ARGS})
#install(FILES kexihandler.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
