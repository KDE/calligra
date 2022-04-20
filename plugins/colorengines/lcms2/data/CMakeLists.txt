add_subdirectory(elles-icc-profiles)
install(FILES
    README
    scRGB.icm
    cmyk.icm
    krita25_lcms-builtin-sRGB_g100-truegamma.icc
    DESTINATION ${KDE_INSTALL_DATAROOTDIR}/color/icc/calligra)
