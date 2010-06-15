
add_subdirectory( common_icon )
add_subdirectory( Screen )
add_subdirectory( exportHTML )
#add_subdirectory( Screenpresentations )
#add_subdirectory( A4 )
#add_subdirectory( legal )
#add_subdirectory( letter )

#set(commondesktopfiles common_desktop/OneColumnLandscape.desktop common_desktop/TitleLandscape.desktop common_desktop/TwoColumnLandscape.desktop common_desktop/emptyLandscape.desktop common_desktop/OneColumnPortrait.desktop common_desktop/TitlePortrait.desktop common_desktop/TwoColumnPortrait.desktop common_desktop/emptyPortrait.desktop)
set(commondesktopfiles common_desktop/emptyLandscape.desktop common_desktop/emptyLandscapeWide.desktop)

foreach (_currentcommondesktopfiles ${commondesktopfiles})
#	install(FILES ${_currentcommondesktopfiles} DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/A4/)
#	install(FILES ${_currentcommondesktopfiles} DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/legal/)
#	install(FILES ${_currentcommondesktopfiles} DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/letter/)
	install(FILES ${_currentcommondesktopfiles} DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/Screen/)
endforeach(_currentcommondesktopfiles ${commondesktopfiles})

