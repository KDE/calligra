#project(planwork)

add_definitions(-DTRANSLATION_DOMAIN=\"calligraplanwork\")

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

ki18n_wrap_ui(planworkfactory_LIB_SRCS
    packagesettings.ui
    taskcompletionpanel.ui
)

kconfig_add_kcfg_files(planworksettings_SRCS calligraplanworksettings.kcfgc)

add_library(planworkfactory SHARED ${planworkfactory_LIB_SRCS} ${planworksettings_SRCS})
generate_export_header(planworkfactory BASE_NAME kplatowork)

target_link_libraries(planworkfactory
    PUBLIC
        kplatokernel
        kplatomodels
        kplatoui
        planprivate
        komain
        KGantt
        KF5::Parts
    PRIVATE
        KF5::IconThemes
)

set_target_properties(planworkfactory PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS planworkfactory ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES calligraplanworksettings.kcfg DESTINATION ${KCFG_INSTALL_DIR})


########### KPlatoWork part ###############

set(planworkpart_PART_SRCS factoryinit.cpp )


add_library(calligraplanworkpart MODULE ${planworkpart_PART_SRCS})
if(NOT ${KF5_VERSION} VERSION_LESS "5.16.0")
    set(calligraplanworkpart_SERVICE_TYPES_ARGS SERVICE_TYPES kpart.desktop)
endif()
kcoreaddons_desktop_to_json(calligraplanworkpart planworkpart.desktop
    ${calligraplanworkpart_SERVICE_TYPES_ARGS}
)

target_link_libraries(calligraplanworkpart Qt5::Core KF5::Parts planworkfactory)

install(TARGETS calligraplanworkpart DESTINATION ${PLUGIN_INSTALL_DIR})


########### Calligra PlanWork executable ###############

set(calligraplanwork_KDEINIT_SRCS main.cpp commandlineparser.cpp)

kf5_add_kdeinit_executable(calligraplanwork ${calligraplanwork_KDEINIT_SRCS})

if (APPLE)
   set_target_properties(calligraplanwork PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligraplanwork PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.plan.work")
   set_target_properties(calligraplanwork PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Plan Work")
endif ()

target_link_libraries(
    kdeinit_calligraplanwork

    planworkfactory
    kplatokernel
    komain
    KF5::Parts
    KF5::IconThemes
    KF5::WindowSystem
    KF5::DBusAddons
)

install(TARGETS kdeinit_calligraplanwork  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( calligraplanwork kdeinit_calligraplanwork )

install(TARGETS calligraplanwork  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  calligraplanwork.rc calligraplanwork_readonly.rc calligraplanworkui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/calligraplanwork)
install( FILES  calligraplanworkrc DESTINATION ${CONFIG_INSTALL_DIR})
install( PROGRAMS  org.kde.calligraplanwork.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})

