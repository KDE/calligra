add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44020)

include_directories( ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( 
-DKEXI_NO_CTXT_HELP=1
)

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
   kexipart.cpp 
   kexiprojectdata.cpp 
   KexiRecentProjects.cpp 
   kexiinternalpart.cpp 
   kexidragobjects.cpp 
   kexistartupdata.cpp 
   kexiguimsghandler.cpp 
   kexitextmsghandler.cpp 
   kexidataiteminterface.cpp 
   kexidbshortcutfile.cpp 
   kexiblobbuffer.cpp 
   kexistaticpart.cpp
   kexitabledesignerinterface.cpp
   kexisearchandreplaceiface.cpp
   kexitemplateloader.cpp
   KexiRecordNavigatorHandler.cpp
   KexiRecordNavigatorIface.cpp
   KexiSearchableModel.cpp
)

kde4_add_library(kexicore SHARED ${kexicore_LIB_SRCS})

target_link_libraries(kexicore
  ${KDE4_KIO_LIBS}
  ${KDE4_KDEUI_LIBS}
  kexiutils
  kexidb
  ${CALLIGRADB_LIBS}
  ${KOPROPERTY_LIBS}
  kowidgetutils
)
target_link_libraries(kexicore LINK_INTERFACE_LIBRARIES kexiutils kexidb ${KOPROPERTY_LIBS})

set_target_properties(kexicore PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS kexicore  ${INSTALL_TARGETS_DEFAULT_ARGS})


