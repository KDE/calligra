kde4_install_icons(${DATA_INSTALL_DIR}/kexi/icons)

install(FILES
        kexi-logo.png
        kexi-logo-white.png
        DESTINATION ${DATA_INSTALL_DIR}/kexi/pics)

add_subdirectory(oxygen)
