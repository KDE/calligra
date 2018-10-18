########### install files ###############

install( FILES  empty-portrait.otg empty-landscape.otg
         DESTINATION ${DATA_INSTALL_DIR}/flow/templates/Basic/.source)
install( FILES  empty-portrait.desktop empty-landscape.desktop
         DESTINATION ${DATA_INSTALL_DIR}/flow/templates/Basic)

ecm_install_icons(ICONS 
    48-actions-template_basicflow.png        
    48-actions-template_empty_portrait.png
    sc-actions-template_empty_landscape.svgz                                                                 
    48-actions-template_empty_landscape.png
    sc-actions-template_basicworkflow.svgz
    sc-actions-template_empty_portrait.svgz
    DESTINATION ${DATA_INSTALL_DIR}/flow/icons  THEME hicolor )
