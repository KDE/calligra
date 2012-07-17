set( QUICK_PRINT_SERVICES
)

if( BUILD_flow )
    set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} flow_konqi.desktop )
endif( BUILD_flow )

if( BUILD_karbon )
    set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} karbon_konqi.desktop )
endif( BUILD_karbon )

if( BUILD_krita )
    set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} kpresenter_konqi.desktop )
endif( BUILD_krita )

if( BUILD_sheets )
    set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} krita_konqi.desktop )
endif( BUILD_sheets )

if( BUILD_stage )
    set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} kspread_konqi.desktop )
endif( BUILD_stage )

if( BUILD_words )
    set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} words_konqi.desktop )
endif( BUILD_words )

install( FILES ${QUICK_PRINT_SERVICES} DESTINATION ${SERVICES_INSTALL_DIR}/ServiceMenus)
