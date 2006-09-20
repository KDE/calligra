
project(kplato)

add_subdirectory( templates )
add_subdirectory( pics )
add_subdirectory( reports )
add_subdirectory( toolbar )
add_subdirectory( tests )

include_directories(${KOTEXT_INCLUDES} ${KOFFICEUI_INCLUDES} ${CMAKE_SOURCE_DIR}/kdgantt ${CMAKE_SOURCE_DIR}/kugar/lib   )


set(libkplatopart_SRCS
    kptsummarytaskdialog.cc
    kptsummarytaskgeneralpanel.cc
    kptschedule.cc
    kptappointment.cc
    kptresourceappointmentsview.cc
    kpttaskappointmentsview.cc
    kptdoublelistviewbase.cc
    kptaccountsviewconfigdialog.cc
    kptaccountsview.cc
    kpttaskcostpanel.cc
    kptmilestoneprogresspanel.cc
    kptmilestoneprogressdialog.cc
    kptaccount.cc
    kptaccountsdialog.cc
    kptaccountspanel.cc
    kptnode.cc
    kptproject.cc
    kptrelation.cc
    kptresource.cc
    kpttask.cc
    kptduration.cc
    kptfactory.cc
    kptpart.cc
    kptview.cc
    kptprojectdialog.cc
    kpttaskdialog.cc
    kptmainprojectdialog.cc
    kptmainprojectpanel.cc
    kptganttview.cc
    kptresourcespanel.cc
    kptpertview.cc
    kptpertcanvas.cc
    kptrelationdialog.cc
    kptcanvasitem.cc
    kptresourceview.cc
    kptrequestresourcespanel.cc
    kptresourcedialog.cc
    kptdatetime.cc
    kptcalendar.cc
    kptcalendaredit.cc
    kptcalendarlistdialog.cc
    kptcalendarpanel.cc
    kptdatetable.cc
    kptcalendarlistdialogbase.cc
    kptcalendareditbase.cc
    kptcommand.cc
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
    kptwbsdefinition.cc
    KPtViewAdaptor.cc
)
kde4_add_ui_files(libkplatopart_SRCS
    standardworktimedialogbase.ui
)
kde4_add_ui3_files(libkplatopart_SRCS
    kpttaskcostpanelbase.ui
    kptsummarytaskgeneralpanelbase.ui
    kptcalendarlistpanel.ui
    kptaccountsviewconfigurepanelbase.ui
    kptmilestoneprogresspanelbase.ui
    kptaccountspanelbase.ui
    kptdurationwidget.ui
    kptmainprojectpanelbase.ui
    relationpanel.ui
    resourcedialogbase.ui
    kpttaskresourcespanelbase.ui
    resourcespanelbase.ui
    kptprojectdialogbase.ui
    kpttasknotespanelbase.ui
    kpttaskgeneralpanelbase.ui
    kptwbsdefinitionpanelbase.ui
    kpttaskprogresspanelbase.ui
    kptconfigtaskpanelbase.ui
    kptintervaleditbase.ui
    )

########### next target ###############

set(kplatopart_PART_SRCS kptfactoryinit.cc ${libkdgantt_SRCS} ${libkplatopart_SRCS} )

kde4_automoc(${kplatopart_PART_SRCS})

kde4_add_plugin(kplatopart WITH_PREFIX ${kplatopart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kplatopart )

target_link_libraries(kplatopart kofficeui ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY}  )

install(TARGETS kplatopart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kplato_KDEINIT_SRCS main.cc )

kde4_automoc(${kplato_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kplato ${kplato_KDEINIT_SRCS})

target_link_libraries(kdeinit_kplato  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_kplato  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kplato kdeinit_kplato )
install(TARGETS kplato  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kplatopart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kplato.rc kplato_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kplato)
install( FILES  kplato.desktop  DESTINATION ${XDG_APPS_DIR})




