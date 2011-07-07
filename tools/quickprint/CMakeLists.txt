
set( QUICK_PRINT_SERVICES kchart_konqi.desktop karbon_konqi.desktop
kspread_konqi.desktop  kpresenter_konqi.desktop
words_konqi.desktop krita_konqi.desktop flow_konqi.desktop )

if( BUILD_kformula )
set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} kformula_konqi.desktop )
endif( BUILD_kformula )

# TODO unsupported for now
# if( BUILD_kexi )
# set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} kexi_konqi.desktop )
# endif( BUILD_kexi )

if( BUILD_flow )
set( QUICK_PRINT_SERVICES ${QUICK_PRINT_SERVICES} flow_konqi.desktop )
endif( BUILD_flow )

install( FILES ${QUICK_PRINT_SERVICES} DESTINATION ${SERVICES_INSTALL_DIR}/ServiceMenus)

