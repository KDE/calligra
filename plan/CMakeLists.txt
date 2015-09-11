
project(kplato)

# set kplato debug area
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=42000 )

if (KF5AkonadiContact_FOUND)
    add_definitions(-DPLAN_KDEPIMLIBS_FOUND)
endif ()

if (PLANCHARTDEBUG)
    add_definitions(-DPLAN_CHART_DEBUG)
endif ()

set( KPLATO_INCLUDES
    ${CMAKE_SOURCE_DIR}/plan/libs/kernel
    ${CMAKE_BINARY_DIR}/plan/libs/kernel
    ${CMAKE_SOURCE_DIR}/plan/libs/models
    ${CMAKE_BINARY_DIR}/plan/libs/models
    ${CMAKE_SOURCE_DIR}/plan/libs/ui
    ${CMAKE_BINARY_DIR}/plan/libs/ui
    ${CMAKE_BINARY_DIR}/plan

    ${KOMAIN_INCLUDES}
    ${KDEPIMLIBS_INCLUDE_DIR}
)

add_subdirectory( libs )

add_subdirectory( templates )
add_subdirectory( pics )
add_subdirectory( toolbar )
add_subdirectory( plugins )
add_subdirectory( tests )

add_subdirectory( workpackage )

include_directories(${KPLATO_INCLUDES})

add_definitions(-DTRANSLATION_DOMAIN=\"plan\")

########### KPlato private library ###############

set(planprivate_LIB_SRCS
    kptviewlistdocker.cpp
    kptviewlist.cpp
    kptviewlistdialog.cpp

    kptschedulesdocker.cpp

    kptconfig.cpp
    kpttaskdefaultpanel.cpp
    kptworkpackageconfigpanel.cpp
    kptcolorsconfigpanel.cpp

    kptcontext.cpp

    kptfactory.cpp
    kptpart.cpp
    kptmaindocument.cpp
    kptview.cpp
#     KPtViewAdaptor.cpp
    kptprintingcontrolprivate.cpp

    kptschedulerpluginloader.cpp
    kptbuiltinschedulerplugin.cpp
    kptconfigskeleton.cpp

    kptinsertfiledlg.cpp

    about/aboutpage.cpp

    KPlatoXmlLoader.cpp
)

ki18n_wrap_ui(planprivate_LIB_SRCS
    kptviewlistaddview.ui
    kptviewlisteditview.ui
    kptviewlisteditcategory.ui

    kptconfigtaskpanelbase.ui
    kptworkpackageconfigpanel.ui
    kptcolorsconfigpanel.ui

    kptinsertfilepanel.ui
)

kconfig_add_kcfg_files(plansettings_SRCS plansettings.kcfgc)

add_library(planprivate SHARED ${planprivate_LIB_SRCS} ${plansettings_SRCS} )
generate_export_header(planprivate BASE_NAME kplato)

target_link_libraries(planprivate kplatokernel kplatomodels kplatoui KF5::KHtml komain)
if(KF5AkonadiContact_FOUND)
    target_link_libraries(planprivate KF5::AkonadiContact)
endif()
set_target_properties(planprivate PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS planprivate ${INSTALL_TARGETS_DEFAULT_ARGS})

########### KPlato part ###############

set(planpart_PART_SRCS kptfactoryinit.cpp )

add_library(planpart MODULE ${planpart_PART_SRCS})

kcoreaddons_desktop_to_json(planpart planpart.desktop)

target_link_libraries(planpart KF5::Parts planprivate)

install(TARGETS planpart DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### KPlato executable ###############

if(NOT RELEASE_BUILD)
    add_definitions(-DMAINTANER_WANTED_SPLASH)
endif()

set(calligraplan_KDEINIT_SRCS main.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/*-apps-calligraplan.png")
ecm_add_app_icon(calligraplan_KDEINIT_SRCS ICONS ${ICONS_SRCS})

kf5_add_kdeinit_executable( calligraplan ${calligraplan_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.plan")
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Plan")
endif ()

target_link_libraries(kdeinit_calligraplan komain)

install(TARGETS kdeinit_calligraplan  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(calligraplan kdeinit_calligraplan komain)
install(TARGETS calligraplan  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  plan.rc plan_readonly.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/plan)
install( PROGRAMS  plan.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES  planrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES plansettings.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install( FILES  plan_viewplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES plan.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)

install(FILES
    about/top-left-plan.png
    about/main.html
    about/intro.html
    about/tips.html
    about/tutorial.html
    about/plan.css
    DESTINATION ${DATA_INSTALL_DIR}/plan/about
)
