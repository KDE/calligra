
add_subdirectory( common_icon )
add_subdirectory( Screen )
add_subdirectory( exportHTML )
add_subdirectory( odf )

set(commondesktopfiles common_desktop/emptyLandscape.desktop common_desktop/emptyLandscapeWide.desktop)

foreach (_currentcommondesktopfiles ${commondesktopfiles})
	install(FILES ${_currentcommondesktopfiles} DESTINATION ${KDE_INSTALL_DATADIR}/calligrastage/templates/Screen/)
endforeach(_currentcommondesktopfiles ${commondesktopfiles})

