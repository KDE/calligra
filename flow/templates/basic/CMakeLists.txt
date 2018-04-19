########### install files ###############

install( FILES  empty-portrait.otg empty-landscape.otg
         DESTINATION ${DATA_INSTALL_DIR}/flow/templates/Basic/.source)
install( FILES  empty-portrait.desktop empty-landscape.desktop
         DESTINATION ${DATA_INSTALL_DIR}/flow/templates/Basic)

ecm_install_icons(ICONS 
    48-action-template_basicflow.png        
    48-action-template_empty_portrait.png
    sc-action-template_empty_landscape.svgz                                                                 
    48-action-template_empty_landscape.png
    sc-action-template_basicworkflow.svgz
    sc-action-template_empty_portrait.svgz
    DESTINATION ${DATA_INSTALL_DIR}/flow/icons  THEME hicolor )
