
project(kplato)

# set kplato debug area
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=42000 )

if(KDE4_BUILD_TESTS)
    # only with this definition will the KPLATO_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

if (KdepimLibs_FOUND)
    add_definitions(-DPLAN_KDEPIMLIBS_FOUND)
endif (KdepimLibs_FOUND)

set( KPLATO_INCLUDES
    ${CMAKE_SOURCE_DIR}/plan/libs/kernel
    ${CMAKE_SOURCE_DIR}/plan/libs/models
    ${CMAKE_BINARY_DIR}/plan/libs/models
    ${CMAKE_SOURCE_DIR}/plan/libs/ui
    ${CMAKE_BINARY_DIR}/plan/libs/ui
    ${CMAKE_BINARY_DIR}/plan
    ${CMAKE_SOURCE_DIR}/kdgantt
    ${CMAKE_BINARY_DIR}/kdgantt
    ${CMAKE_SOURCE_DIR}/plugins/chartshape/kdchart/include

    ${KOMAIN_INCLUDES}    
    ${KDEPIMLIBS_INCLUDE_DIR}

    ${CMAKE_SOURCE_DIR}/libs/koreport
    ${CMAKE_SOURCE_DIR}/libs/koreport/common
    ${CMAKE_SOURCE_DIR}/libs/koreport/renderer
    ${CMAKE_SOURCE_DIR}/libs/koreport/wrtembed
)

add_subdirectory( libs )

add_subdirectory( templates )
add_subdirectory( pics )
add_subdirectory( toolbar )
add_subdirectory( plugins )
add_subdirectory( tests )

add_subdirectory( workpackage )

include_directories(${KPLATO_INCLUDES})


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
    kptpartpart.cpp
    kptpart.cpp
    kptview.cpp
#     KPtViewAdaptor.cpp

    kptschedulerpluginloader.cpp
    kptbuiltinschedulerplugin.cpp
    kptconfigskeleton.cpp

    kptinsertfiledlg.cpp

    about/aboutpage.cpp

    KPlatoXmlLoader.cpp
)

kde4_add_ui_files(planprivate_LIB_SRCS
    kptviewlistaddview.ui
    kptviewlisteditview.ui
    kptviewlisteditcategory.ui

    kptconfigtaskpanelbase.ui
    kptworkpackageconfigpanel.ui
    kptcolorsconfigpanel.ui

    kptinsertfilepanel.ui
)

kde4_add_kcfg_files(plansettings_SRCS plansettings.kcfgc)

kde4_add_library(planprivate SHARED ${planprivate_LIB_SRCS} ${libkdgantt_SRCS} ${plansettings_SRCS} )

target_link_libraries(planprivate kplatokernel kplatomodels kplatoui ${KDE4_KHTML_LIBS} ${KDEPIMLIBS_AKONADI_CONTACT_LIBS}  komain)

set_target_properties(planprivate PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS planprivate ${INSTALL_TARGETS_DEFAULT_ARGS})

########### KPlato part ###############

set(planpart_PART_SRCS kptfactoryinit.cpp )

kde4_add_plugin(planpart ${planpart_PART_SRCS})

target_link_libraries(planpart ${KDE4_KPARTS_LIBS} planprivate)

install(TARGETS planpart DESTINATION ${PLUGIN_INSTALL_DIR})


########### KPlato executable ###############

set(calligraplan_KDEINIT_SRCS main.cpp )

kde4_add_app_icon(calligraplan_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-calligraplan.png")

kde4_add_kdeinit_executable( calligraplan ${calligraplan_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.plan")
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Plan")
endif (Q_WS_MAC)

target_link_libraries(kdeinit_calligraplan komain)

install(TARGETS kdeinit_calligraplan  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(calligraplan kdeinit_calligraplan komain)
install(TARGETS calligraplan  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  planpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  plan.rc plan_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/plan)
install( PROGRAMS  plan.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES  planrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES plansettings.kcfg DESTINATION ${KCFG_INSTALL_DIR})

install(FILES
    about/top-left-plan.png
    about/main.html
    about/intro.html
    about/tips.html
    about/tutorial.html
    about/plan.css
    DESTINATION ${DATA_INSTALL_DIR}/plan/about
)

