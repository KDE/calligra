set(oxygen_icon_dirs 16x16 22x22  32x32 48x48 64x64 128x128 scalable )
install( DIRECTORY ${oxygen_icon_dirs} DESTINATION ${DATA_INSTALL_DIR}/kexi/icons/oxygen PATTERN .svn EXCLUDE )
