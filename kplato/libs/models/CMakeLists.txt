
#add_subdirectory( tests )

include_directories(
    ../kernel 
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/kdgantt
    ${KDE4_INCLUDES}
    ${KDEPIMLIBS_INCLUDE_DIR}
)

add_subdirectory( tests )

########### KPlato data models library ###############

set(kplatomodels_LIB_SRCS
    kptcommonstrings.cpp
    kpttreecombobox.cpp

    kcalendar/kdatetable.cpp
    kcalendar/kdatepicker.cpp
    
    kptabstractchartmodel.cpp
    kptnodechartmodel.cpp
    
    kptrelationmodel.cpp
    kptworkpackagemodel.cpp
    kptdocumentmodel.cpp
    kptitemmodelbase.cpp
    kptnodeitemmodel.cpp
    kptdurationspinbox.cpp
    kpttaskstatusmodel.cpp
    kptresourcemodel.cpp
    kptcalendarmodel.cpp
    kptschedulemodel.cpp
    kptaccountsmodel.cpp
    kptpertcpmmodel.cpp
    kptresourceappointmentsmodel.cpp
    kptresourceallocationmodel.cpp

    kpttaskcompletedelegate.cpp
)

kde4_add_library(kplatomodels SHARED ${kplatomodels_LIB_SRCS})

target_link_libraries(kplatomodels kplatokernel ${KDE4_KDEUI_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KABC_LIBS})

set_target_properties(kplatomodels PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatomodels ${INSTALL_TARGETS_DEFAULT_ARGS})

