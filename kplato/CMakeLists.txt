
project(kplato)

if(KDE4_BUILD_TESTS)
    # only with this definition will the KPLATO_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

include (../kdgantt/CMakeLists.txt)

set( KPLATO_INCLUDES
    ${CMAKE_SOURCE_DIR}/kplato/libs/kernel
    ${CMAKE_SOURCE_DIR}/kplato/libs/models
    ${KOGUIUTILS_INCLUDES}
    ${CMAKE_SOURCE_DIR}/kdgantt
    ${CMAKE_BINARY_DIR}/kdgantt
)

add_subdirectory( libs )

add_subdirectory( templates )
add_subdirectory( pics )
add_subdirectory( reports )
add_subdirectory( toolbar )
add_subdirectory( tests )

add_subdirectory( workpackage )

include_directories(${KPLATO_INCLUDES})

########### KPlato private library ###############

set(kplatoprivate_LIB_SRCS
    viewlist/kptviewlistdocker.cc
    viewlist/kptviewlist.cc
    viewlist/kptviewlistdialog.cc
    viewlist/kptviewlistcommand.cc
    
    kptitemviewsettup.cc
    kptsplitterview.cc
    kptdependencyeditor.cc
    kptusedefforteditor.cc
    kpttaskstatusview.cc
    kptcalendareditor.cc
    kptviewbase.cc
    kptaccountseditor.cc
    kptperteditor.cc
    kptpertresult.cc
    kpttaskeditor.cc
    kptresourceeditor.cc
    kptscheduleeditor.cc
    kptsummarytaskdialog.cc
    kptsummarytaskgeneralpanel.cc
    kptresourceappointmentsview.cc
    kpttaskappointmentsview.cc
    kptdoublelistviewbase.cc
    kptaccountsviewconfigdialog.cc
    kptaccountsview.cc
    kpttaskcostpanel.cc
    kptmilestoneprogresspanel.cc
    kptmilestoneprogressdialog.cc
    kptaccountsdialog.cc
    kptaccountspanel.cc
    kptfactory.cc
    kptpart.cc
    kptview.cc
    kpttaskdialog.cc
    kptmainprojectdialog.cc
    kptmainprojectpanel.cc
    kptganttview.cc
    kptresourcespanel.cc
    kptrelationdialog.cc
    kptrequestresourcespanel.cc
    kptresourcedialog.cc
    kptcalendaredit.cc
    kptcalendarlistdialog.cc
    kptcalendarpanel.cc
    kptdatetable.cc
    kptcalendarlistdialogbase.cc
    kptcalendareditbase.cc
    kptstandardworktimedialog.cc
    kptintervaledit.cc
    kpttaskgeneralpanel.cc
    kpttaskdefaultpanel.cc
    kptconfigdialog.cc
    kpttaskprogresspanel.cc
    kpttaskprogressdialog.cc
    kptconfig.cc
    kptcontext.cc
    kptresourcesdialog.cc
    kptwbsdefinitiondialog.cc
    kptwbsdefinitionpanel.cc
    KPtViewAdaptor.cc
    kptresourceassignmentview.cc
    kptchartpanel.cc
    kptchartwidget.cc
    kptchart.cc
    kptchartview.cc
)
kde4_add_ui_files(kplatoprivate_LIB_SRCS
    viewlist/kptviewlistaddview.ui
    viewlist/kptviewlisteditview.ui
    viewlist/kptviewlisteditcategory.ui
    
    kptcpmwidget.ui
    kptitemviewsettings.ui
    kptpertresult.ui
    standardworktimedialogbase.ui
    kptwbsdefinitionpanelbase.ui
    kptaccountspanelbase.ui
    resourcespanelbase.ui
    kptaccountsviewconfigurepanelbase.ui
    kptintervaleditbase.ui
    kpttaskresourcespanelbase.ui
    kpttaskcostpanelbase.ui
    kpttasknotespanelbase.ui
    kptsummarytaskgeneralpanelbase.ui
    kptmilestoneprogresspanelbase.ui
    resourcedialogbase.ui
    kptmainprojectpanelbase.ui
    kptcalendarlistpanel.ui
    relationpanel.ui
    kpttaskgeneralpanelbase.ui
    kpttaskprogresspanelbase.ui
    kptconfigtaskpanelbase.ui
    kptperteditor.ui
    kptchartpanelbase.ui
    kptresourceassignmentview.ui
)

kde4_add_library(kplatoprivate SHARED ${kplatoprivate_LIB_SRCS} ${libkdgantt_SRCS})

target_link_libraries(kplatoprivate kplatokernel kplatomodels koguiutils  )

set_target_properties(kplatoprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatoprivate DESTINATION ${LIB_INSTALL_DIR})

########### KPlato part ###############

set(kplatopart_PART_SRCS kptfactoryinit.cc )


kde4_add_plugin(kplatopart WITH_PREFIX ${kplatopart_PART_SRCS})

target_link_libraries(kplatopart kplatoprivate)

install(TARGETS kplatopart DESTINATION ${PLUGIN_INSTALL_DIR})


########### KPlato executable ###############

set(kplato_KDEINIT_SRCS main.cc )

kde4_add_kdeinit_executable( kplato ${kplato_KDEINIT_SRCS})

target_link_libraries(kdeinit_kplato komain )

install(TARGETS kdeinit_kplato  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kplato kdeinit_kplato )
install(TARGETS kplato  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kplatopart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kplato.rc kplato_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/kplato)
install( FILES  kplato.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})




