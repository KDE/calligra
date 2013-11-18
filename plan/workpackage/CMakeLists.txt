#project(planwork)

add_subdirectory( templates )
add_subdirectory( pics )
#add_subdirectory( toolbar )
#add_subdirectory( tests )

include_directories(${KPLATO_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/)


########### KPlatoWork private library ###############

set(planworkfactory_LIB_SRCS
    factory.cpp
    mainwindow.cpp
    part.cpp
    view.cpp
    workpackage.cpp

    taskworkpackagemodel.cpp
    taskworkpackageview.cpp

    packagesettings.cpp
    taskcompletiondialog.cpp

    debugarea.cpp

    ../KPlatoXmlLoader.cpp #NB
)

kde4_add_ui_files(planworkfactory_LIB_SRCS
    packagesettings.ui
    taskcompletionpanel.ui
)

kde4_add_kcfg_files(planworksettings_SRCS planworksettings.kcfgc)

kde4_add_library(planworkfactory SHARED ${planworkfactory_LIB_SRCS} ${planworksettings_SRCS})

target_link_libraries(
    planworkfactory
    kplatokernel
    kplatomodels
    kplatoui
    planprivate
    komain
    calligrakdgantt
    ${KDE4_KPARTS_LIBS}
    )

set_target_properties(planworkfactory PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS planworkfactory ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES planworksettings.kcfg DESTINATION ${KCFG_INSTALL_DIR})


########### KPlatoWork part ###############

set(planworkpart_PART_SRCS factoryinit.cpp )


kde4_add_plugin(planworkpart ${planworkpart_PART_SRCS})

target_link_libraries(planworkpart ${QT_QTCORE_LIBRARY} ${KDE4_KPARTS_LIBS} planworkfactory)

install(TARGETS planworkpart DESTINATION ${PLUGIN_INSTALL_DIR})


########### KPlatoWork application ###############

set(planworkapp_LIB_SRCS application.cpp )


kde4_add_library(planworkapp  SHARED ${planworkapp_LIB_SRCS})

target_link_libraries(
    planworkapp
    planworkfactory
    kplatokernel
    komain
    )

set_target_properties(planworkapp PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS planworkapp ${INSTALL_TARGETS_DEFAULT_ARGS})


########### Calligra PlanWork executable ###############

set(calligraplanwork_KDEINIT_SRCS main.cpp)

kde4_add_kdeinit_executable(calligraplanwork ${calligraplanwork_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(calligraplanwork PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligraplanwork PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.plan.work")
   set_target_properties(calligraplanwork PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Plan Work")
endif (Q_WS_MAC)

target_link_libraries(
    kdeinit_calligraplanwork
    planworkapp
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${KDE4_KPARTS_LIBS}
    )

install(TARGETS kdeinit_calligraplanwork  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( calligraplanwork kdeinit_calligraplanwork )

install(TARGETS calligraplanwork  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  planworkpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  planwork.rc planwork_readonly.rc planwork_mainwindow.rc DESTINATION ${DATA_INSTALL_DIR}/planwork)
install( FILES  planworkrc DESTINATION ${CONFIG_INSTALL_DIR})
install( PROGRAMS  planwork.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})

