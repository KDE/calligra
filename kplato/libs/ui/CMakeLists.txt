
include (${CMAKE_SOURCE_DIR}/kdgantt/CMakeLists.txt)

include_directories(
    ${CMAKE_SOURCE_DIR}/kplato/libs/kernel
    ${CMAKE_SOURCE_DIR}/kplato/libs/models
    ${CMAKE_BINARY_DIR}/kplato/libs/models
    ${CMAKE_SOURCE_DIR}/kdgantt
    ${CMAKE_BINARY_DIR}/kdgantt
    ${KOMAIN_INCLUDES}    
    ${KDEPIMLIBS_INCLUDE_DIR}
)

#add_subdirectory( tests )

########### KPlato private library ###############

set(kplatoui_LIB_SRCS
    plotting/kplotaxis.cpp
    plotting/kplotwidget.cpp
    plotting/kplotobject.cpp
    
    kptganttitemdelegate.cpp

    kptworkpackagesendpanel.cpp
    kptworkpackagesenddialog.cpp
    
    kptdocumentseditor.cpp
    kptdocumentspanel.cpp
    
    kptitemviewsettup.cpp
    kptsplitterview.cpp
    kptrelationeditor.cpp
    kptdependencyeditor.cpp
    kptusedefforteditor.cpp
    kpttaskstatusview.cpp
    kptcalendareditor.cpp
    kptviewbase.cpp
    kptaccountseditor.cpp
    kptperteditor.cpp
    kptpertresult.cpp
    kpttaskeditor.cpp
    kptresourceeditor.cpp
    kptscheduleeditor.cpp
    kptsummarytaskdialog.cpp
    kptsummarytaskgeneralpanel.cpp
    kptresourceappointmentsview.cpp
    kptaccountsviewconfigdialog.cpp
    kptaccountsview.cpp
    kpttaskcostpanel.cpp
    kptmilestoneprogresspanel.cpp
    kptmilestoneprogressdialog.cpp
    kpttaskdialog.cpp
    kptmainprojectdialog.cpp
    kptmainprojectpanel.cpp
    kptganttview.cpp
    kptrelationdialog.cpp
    kptrequestresourcespanel.cpp
    kptresourcedialog.cpp
    kptstandardworktimedialog.cpp
    kptintervaledit.cpp
    kpttaskgeneralpanel.cpp
    kpttaskprogresspanel.cpp
    kpttaskprogressdialog.cpp
    kpttaskdescriptiondialog.cpp
    kptwbsdefinitiondialog.cpp
    kptwbsdefinitionpanel.cpp
    kptresourceassignmentview.cpp
    kptchartpanel.cpp
    kptchartwidget.cpp
    kptchart.cpp
    kptchartview.cpp
    kptplotwidget.cpp

    kptresourceallocationeditor.cpp

    kptrecalculatedialog.cpp

    kpthtmlview.cpp

    locale/localemon.cpp
    kptlocaleconfigmoneydialog.cpp
)
kde4_add_ui_files(kplatoui_LIB_SRCS
    kptresourceappointmentsdisplayoptions.ui
    kptganttchartdisplayoptions.ui

    kptprintingheaderfooter.ui
    kptganttprintingoptions.ui
    
    kptworkpackagesendpanel.ui
    
    kptdocumentspanel.ui
    
    kptperformancestatus.ui
    
    kptcpmwidget.ui
    kptitemviewsettings.ui
    kptpertresult.ui
    standardworktimedialogbase.ui
    kptwbsdefinitionpanelbase.ui
    kptaccountsviewconfigurepanelbase.ui
    kptintervaleditbase.ui
    kpttaskcostpanelbase.ui
    kpttaskdescriptionpanelbase.ui
    kptsummarytaskgeneralpanelbase.ui
    kptmilestoneprogresspanelbase.ui
    resourcedialogbase.ui
    kptmainprojectpanelbase.ui
    relationpanel.ui
    kpttaskgeneralpanelbase.ui
    kpttaskprogresspanelbase.ui
    kptperteditor.ui
    kptchartpanelbase.ui
    kptresourceassignmentview.ui
    kpttaskstatusviewsettingspanel.ui
    kptperformancestatusviewsettingspanel.ui

    kptrecalculatedialog.ui

    locale/localemon.ui
)
add_definitions(-DKDGANTT_STATICLIB)

kde4_add_library(kplatoui SHARED ${kplatoui_LIB_SRCS} ${libkdgantt_SRCS})

target_link_libraries(kplatoui ${KDE4_KABC_LIBS} komain kplatokernel kplatomodels  ${KDE4_KHTML_LIBS} ${QT_QT3SUPPORT_LIBRARY} )

set_target_properties(kplatoui PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatoui ${INSTALL_TARGETS_DEFAULT_ARGS})




