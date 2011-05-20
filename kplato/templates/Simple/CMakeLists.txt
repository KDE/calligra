install( FILES
        Plain.plant
        8HourDay-40HourWeek.plant
        Basicproject.plant
    DESTINATION ${DATA_INSTALL_DIR}/plan/templates/Simple/.source
)
install( FILES
        .directory
        Plain.desktop
        8HourDay-40HourWeek.desktop
        Basicproject.desktop
    DESTINATION ${DATA_INSTALL_DIR}/plan/templates/Simple
)

kde4_install_icons( ${DATA_INSTALL_DIR}/plan/icons  )
