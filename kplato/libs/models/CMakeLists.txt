
#add_subdirectory( tests )

include_directories(
    ../kernel 
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/kdgantt
)

########### KPlato data models library ###############

set(kplatomodels_LIB_SRCS
    kptdocumentmodel.cpp
    kptitemmodelbase.cpp
    kptnodeitemmodel.cpp
    kptdurationwidget.cpp
    kptdurationspinbox.cpp
    kpttaskstatusmodel.cpp
)

kde4_add_ui_files(kplatomodels_LIB_SRCS
    kptdurationwidget.ui
)

kde4_add_library(kplatomodels SHARED ${kplatomodels_LIB_SRCS})

target_link_libraries(kplatomodels kplatokernel)

set_target_properties(kplatomodels PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatomodels DESTINATION ${LIB_INSTALL_DIR})

