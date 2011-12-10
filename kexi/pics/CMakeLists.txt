kde4_install_icons(${DATA_INSTALL_DIR}/kexi/icons)

install(FILES
        kexi-logo.png
        kexi-logo-white.png
        calligra-logo-white-glow.png
        DESTINATION ${DATA_INSTALL_DIR}/kexi/pics)

add_subdirectory(oxygen)
