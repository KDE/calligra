
include (${CMAKE_SOURCE_DIR}/kdgantt/CMakeLists.txt)

include_directories(
    ${CMAKE_SOURCE_DIR}/kplato/libs/kernel
    ${CMAKE_SOURCE_DIR}/kplato/libs/models
    ${CMAKE_BINARY_DIR}/kplato/libs/models
    ${CMAKE_SOURCE_DIR}/kdgantt
    ${CMAKE_BINARY_DIR}/kdgantt
    ${KOGUIUTILS_INCLUDES}
)

#add_subdirectory( tests )

########### KPlato private library ###############

set(kplatoui_LIB_SRCS
    kptdocumentseditor.cc
    kptdocumentspanel.cc
    
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
    kpttaskprogresspanel.cc
    kpttaskprogressdialog.cc
    kptresourcesdialog.cc
    kptwbsdefinitiondialog.cc
    kptwbsdefinitionpanel.cc
    kptresourceassignmentview.cc
    kptchartpanel.cc
    kptchartwidget.cc
    kptchart.cc
    kptchartview.cc
)
kde4_add_ui_files(kplatoui_LIB_SRCS
    kptdocumentspanel.ui
    
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
    kptperteditor.ui
    kptchartpanelbase.ui
    kptresourceassignmentview.ui
)

kde4_add_library(kplatoui SHARED ${kplatoui_LIB_SRCS} ${libkdgantt_SRCS})

target_link_libraries(kplatoui kplatokernel kplatomodels koguiutils  )

set_target_properties(kplatoui PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatoui DESTINATION ${LIB_INSTALL_DIR})




