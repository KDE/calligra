
#add_subdirectory( tests )

include_directories(
    ../kernel 
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/kdgantt
    ${KDE4_INCLUDES}
)

########### KPlato data models library ###############

set(kplatomodels_LIB_SRCS
    kcalendar/kdatetable.cpp
    kcalendar/kdatepicker.cpp
    
    kptabstractchartmodel.cpp
    
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
)

kde4_add_library(kplatomodels SHARED ${kplatomodels_LIB_SRCS})

target_link_libraries(kplatomodels kplatokernel ${KDE4_KABC_LIBS})

set_target_properties(kplatomodels PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatomodels DESTINATION ${LIB_INSTALL_DIR})

