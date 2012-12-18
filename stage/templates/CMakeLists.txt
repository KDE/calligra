
add_subdirectory( common_icon )
add_subdirectory( Screen )
add_subdirectory( exportHTML )
add_subdirectory( odf )

set(commondesktopfiles common_desktop/emptyLandscape.desktop common_desktop/emptyLandscapeWide.desktop)

foreach (_currentcommondesktopfiles ${commondesktopfiles})
	install(FILES ${_currentcommondesktopfiles} DESTINATION ${DATA_INSTALL_DIR}/stage/templates/Screen/)
	install(FILES ${_currentcommondesktopfiles} DESTINATION ${DATA_INSTALL_DIR}/stage/templates/Svg/)
endforeach(_currentcommondesktopfiles ${commondesktopfiles})


add_subdirectory(Svg)