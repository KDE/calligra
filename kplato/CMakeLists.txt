
project(kplato)

if(KDE4_BUILD_TESTS)
    # only with this definition will the KPLATO_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

set( KPLATO_INCLUDES
    ${CMAKE_SOURCE_DIR}/kplato/libs/kernel
    ${CMAKE_SOURCE_DIR}/kplato/libs/models
    ${CMAKE_BINARY_DIR}/kplato/libs/models
    ${CMAKE_SOURCE_DIR}/kplato/libs/ui
    ${CMAKE_BINARY_DIR}/kplato/libs/ui
    ${CMAKE_BINARY_DIR}/kplato
    ${CMAKE_SOURCE_DIR}/kdgantt
    ${CMAKE_BINARY_DIR}/kdgantt
    ${KOMAIN_INCLUDES}    
    ${KDEPIMLIBS_INCLUDE_DIR}
)

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

add_subdirectory( libs )

add_subdirectory( templates )
add_subdirectory( pics )
add_subdirectory( toolbar )
add_subdirectory( plugins )
#add_subdirectory( tests )

add_subdirectory( workpackage )

include_directories(${KPLATO_INCLUDES})


########### KPlato private library ###############

set(kplatoprivate_LIB_SRCS
    kptviewlistdocker.cpp
    kptviewlist.cpp
    kptviewlistdialog.cpp
    
    kptschedulesdocker.cpp

    kptconfig.cpp
    kpttaskdefaultpanel.cpp
    kptworkpackageconfigpanel.cpp

    kptcontext.cpp

    kptfactory.cpp
    kptpart.cpp
    kptview.cpp
    KPtViewAdaptor.cpp

    kptschedulerpluginloader.cpp
    kptbuiltinschedulerplugin.cpp
    kptconfigskeleton.cpp

    kptinsertfiledlg.cpp

    about/aboutpage.cpp
)

kde4_add_ui_files(kplatoprivate_LIB_SRCS
    kptviewlistaddview.ui
    kptviewlisteditview.ui
    kptviewlisteditcategory.ui

    kptconfigtaskpanelbase.ui
    kptworkpackageconfigpanel.ui

    kptinsertfilepanel.ui
)

kde4_add_kcfg_files(kplatosettings_SRCS kplatosettings.kcfgc)

kde4_add_library(kplatoprivate SHARED ${kplatoprivate_LIB_SRCS} ${libkdgantt_SRCS} ${kplatosettings_SRCS} )

target_link_libraries(kplatoprivate koodf ${KDE4_KPARTS_LIBS} ${KDE4_KABC_LIBS} kplatokernel kplatomodels kplatoui  ${KDE4_KHTML_LIBS} ${KDE4_KPIMUTILS_LIBS} ${QT_QT3SUPPORT_LIBRARY} ${QT_QTCORE_LIBRARY} komain )

set_target_properties(kplatoprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatoprivate ${INSTALL_TARGETS_DEFAULT_ARGS})

########### KPlato part ###############

set(kplatopart_PART_SRCS kptfactoryinit.cpp )

kde4_add_plugin(kplatopart WITH_PREFIX ${kplatopart_PART_SRCS})

target_link_libraries(kplatopart ${KDE4_KPARTS_LIBS} kplatoprivate)

install(TARGETS kplatopart DESTINATION ${PLUGIN_INSTALL_DIR})


########### KPlato executable ###############

set(kplato_KDEINIT_SRCS main.cpp )

kde4_add_app_icon(kplato_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-kplato.png")

kde4_add_kdeinit_executable( kplato ${kplato_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(kplato PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(kplato PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.koffice.kplato")
   set_target_properties(kplato PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "KPlato 2")
endif (Q_WS_MAC)

target_link_libraries(kdeinit_kplato komain  ${KDE4_KDEUI_LIBS})

install(TARGETS kdeinit_kplato  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( kplato kdeinit_kplato ${KDE4_KDEUI_LIBS} komain kostore )
install(TARGETS kplato  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  kplatopart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kplato.rc kplato_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/kplato)
install( FILES  kplato.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES  kplatorc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES kplatosettings.kcfg DESTINATION ${KCFG_INSTALL_DIR})

install(FILES
    about/top-left-kplato.png
    about/main.html
    about/intro.html
    about/tips.html
    about/tutorial.html
    about/kplato.css
    DESTINATION ${DATA_INSTALL_DIR}/kplato/about
)

