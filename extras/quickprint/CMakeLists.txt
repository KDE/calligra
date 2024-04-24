
set( QUICK_PRINT_SERVICES
)

if( SHOULD_BUILD_APP_KARBON )
    set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} karbon_print.desktop )
endif()

if( SHOULD_BUILD_APP_SHEETS )
    set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} sheets_print.desktop )
endif()

if( SHOULD_BUILD_APP_STAGE )
    set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} stage_print.desktop )
endif()

if( SHOULD_BUILD_APP_WORDS )
    set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} words_print.desktop )
endif()

install( FILES ${QUICK_PRINT_SERVICES} DESTINATION ${KDE_INSTALL_DATADIR}/kio/servicemenus)
