#project(kplatowork)

add_subdirectory( templates )
add_subdirectory( pics )
#add_subdirectory( toolbar )
#add_subdirectory( tests )

include_directories(${KPLATO_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/)


########### KPlatoWork private library ###############

set(kplatoworkfactory_LIB_SRCS
    factory.cpp
    mainwindow.cpp
    part.cpp
    view.cpp
    workpackage.cpp

    taskworkpackagemodel.cpp
    taskworkpackageview.cpp

    packagesettings.cpp
    taskcompletiondialog.cpp
)

kde4_add_ui_files(kplatoworkfactory_LIB_SRCS
    packagesettings.ui
    taskcompletionpanel.ui
)

kde4_add_library(kplatoworkfactory SHARED ${kplatoworkfactory_LIB_SRCS})

target_link_libraries(
    kplatoworkfactory
    kplatokernel
    kplatomodels
    kplatoui
    kplatoprivate

    komain
    kostore
    koodf
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

set_target_properties(kplatoworkfactory PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatoworkfactory ${INSTALL_TARGETS_DEFAULT_ARGS})


########### KPlatoWork part ###############

set(kplatoworkpart_PART_SRCS factoryinit.cpp )


kde4_add_plugin(kplatoworkpart WITH_PREFIX ${kplatoworkpart_PART_SRCS})

target_link_libraries(kplatoworkpart ${QT_QTCORE_LIBRARY} ${KDE4_KPARTS_LIBS} kplatoworkfactory)

install(TARGETS kplatoworkpart DESTINATION ${PLUGIN_INSTALL_DIR})


########### KPlatoWork application ###############

set(kplatoworkapp_LIB_SRCS application.cpp )


kde4_add_library(kplatoworkapp  SHARED ${kplatoworkapp_LIB_SRCS})

target_link_libraries(
    kplatoworkapp
    kplatoworkfactory
    kplatokernel
    komain
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${QT_QTCORE_LIBRARY} 
    ${QT_QT3SUPPORT_LIBRARY}   
    )

set_target_properties(kplatoworkapp PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatoworkapp ${INSTALL_TARGETS_DEFAULT_ARGS})


########### KPlatoWork executable ###############

set(kplatowork_KDEINIT_SRCS main.cpp)

kde4_add_kdeinit_executable( kplatowork ${kplatowork_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(kplatowork PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(kplatowork PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.koffice.kplato.work")
   set_target_properties(kplatowork PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "KPlato Work 2")
endif (Q_WS_MAC)

target_link_libraries(
    kdeinit_kplatowork
    kplatoworkapp
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kdeinit_kplatowork  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( kplatowork kdeinit_kplatowork )

install(TARGETS kplatowork  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  kplatoworkpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kplatowork.rc kplatowork_readonly.rc kplatowork_mainwindow.rc DESTINATION ${DATA_INSTALL_DIR}/kplatowork)
install( FILES  kplatoworkrc DESTINATION ${CONFIG_INSTALL_DIR})
install( FILES  kplatowork.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})




