
project(kplato)

# set kplato debug area
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=42000 )

if(KF5Holidays_FOUND)
    add_definitions(-DHAVE_KHOLIDAYS)
endif()

if (KF5AkonadiContact_FOUND)
    # disable for now: there is a bug
    # it only works if you use kde contacts (of course) but many use other stuff, so gets dissapointed
    # add_definitions(-DPLAN_KDEPIMLIBS_FOUND)
    message(WARNING "AkonadiContacs available, but funtion is disabled due to Bug 311940")
endif ()

#### Disable kreport, reconsidder when stable ####
# if (KReport_FOUND AND KReport_VERSION_MAJOR EQUAL 3 AND KReport_VERSION_MINOR EQUAL 0 AND KReport_VERSION_PATCH LESS 80)
#     message(STATUS "Acceptable KReport version: ${KReport_VERSION}")
#     add_definitions(-DPLAN_USE_KREPORT)
#     set(PLAN_BUILD_REPORTS true)
# else()
#     message(WARNING "Unacceptable KReport version: ${KReport_VERSION}")
# endif()

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
)

add_subdirectory( libs )

add_subdirectory( templates )
add_subdirectory( pics )
add_subdirectory( toolbar )
add_subdirectory( plugins )
add_subdirectory( tests )

add_subdirectory( workpackage )

include_directories(${KPLATO_INCLUDES})

add_definitions(-DTRANSLATION_DOMAIN=\"calligraplan\")

########### KPlato private library ###############

set(planprivate_LIB_SRCS
    kptviewlistdocker.cpp
    kptviewlist.cpp
    kptviewlistdialog.cpp

    kptschedulesdocker.cpp

    kptconfig.cpp
    ConfigWorkVacationPanel.cpp
    ConfigProjectPanel.cpp
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

    ConfigWorkVacationPanel.ui
    ConfigProjectPanel.ui
    kptconfigtaskpanelbase.ui
    kptworkpackageconfigpanel.ui
    kptcolorsconfigpanel.ui

    kptinsertfilepanel.ui
)

kconfig_add_kcfg_files(plansettings_SRCS calligraplansettings.kcfgc)

add_library(planprivate SHARED ${planprivate_LIB_SRCS} ${plansettings_SRCS} )
generate_export_header(planprivate BASE_NAME kplato)

target_link_libraries(planprivate
    PUBLIC
        kplatokernel
        kplatomodels
        kplatoui
        komain
    PRIVATE
        koplugin
        KF5::IconThemes
        #KF5::KHtml
)
if(KF5AkonadiContact_FOUND)
    target_link_libraries(planprivate  PRIVATE KF5::AkonadiContact)
endif()
set_target_properties(planprivate PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS planprivate ${INSTALL_TARGETS_DEFAULT_ARGS})

########### KPlato part ###############

set(planpart_PART_SRCS kptfactoryinit.cpp )

add_library(calligraplanpart MODULE ${planpart_PART_SRCS})

calligra_part_desktop_to_json(calligraplanpart planpart.desktop)

target_link_libraries(calligraplanpart KF5::Parts planprivate)

install(TARGETS calligraplanpart DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/parts)


########### KPlato executable ###############

if(NOT RELEASE_BUILD)
    add_definitions(-DMAINTANER_WANTED_SPLASH)
endif()

set(calligraplan_KDEINIT_SRCS main.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/*-apps-calligraplan.png")
ecm_add_app_icon(calligraplan_KDEINIT_SRCS ICONS ${ICONS_SRCS})

kf5_add_kdeinit_executable( calligraplan ${calligraplan_KDEINIT_SRCS})

if (APPLE)
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.plan")
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Plan")
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/calligraplan_KDEINIT_SRCS.icns DESTINATION ${BUNDLE_INSTALL_DIR}/calligraplan.app/Contents/Resources)
endif ()

target_link_libraries(kdeinit_calligraplan komain)

install(TARGETS kdeinit_calligraplan  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(calligraplan kdeinit_calligraplan komain)
install(TARGETS calligraplan  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  calligraplan.rc calligraplan_readonly.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/calligraplan)
install( PROGRAMS  org.kde.calligraplan.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES  calligraplanrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES calligraplansettings.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES org.kde.calligraplan.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# plan_viewplugin.desktop

install(FILES
    about/top-left-plan.png
    about/main.html
    about/intro.html
    about/tips.html
    about/tutorial.html
    about/plan.css
    DESTINATION ${DATA_INSTALL_DIR}/calligraplan/about
)
