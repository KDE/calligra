install( FILES
        Plain.plant
        8HourDay-40HourWeek.plant
        Basicproject.plant
    DESTINATION ${DATA_INSTALL_DIR}/calligraplan/templates/Simple/.source
)
install( FILES
        .directory
        Plain.desktop
        8HourDay-40HourWeek.desktop
        Basicproject.desktop
    DESTINATION ${DATA_INSTALL_DIR}/calligraplan/templates/Simple
)

ecm_install_icons( ICONS
    48-actions-template_timechart.png
    sc-actions-template_timechart.svgz

    DESTINATION ${DATA_INSTALL_DIR}/calligraplan/icons
    THEME hicolor
)
