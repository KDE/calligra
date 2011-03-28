kde4_install_icons(${DATA_INSTALL_DIR}/kexi/icons)

if(KEXI_MODERN_STARTUP)
    install(FILES
            kexi-logo.png
            kexi-logo-white.png
            DESTINATION ${DATA_INSTALL_DIR}/kexi/pics)
endif(KEXI_MODERN_STARTUP)

add_subdirectory(oxygen)
