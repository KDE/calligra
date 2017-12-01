add_subdirectory( Simple )

# Special templates, should never be seen by user
install(FILES SharedResources.plant
    DESTINATION ${DATA_INSTALL_DIR}/calligraplan/templates/.source
)
