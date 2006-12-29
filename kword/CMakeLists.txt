project(kword)

#add_subdirectory( mailmerge )
add_subdirectory( templates )
add_subdirectory( pics )
#add_subdirectory( expression )
#add_subdirectory( data )
#add_subdirectory( tests )
add_subdirectory( part )
add_subdirectory( plugins )

# ########### install files ###############
#
install( FILES  kwmailmerge.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})
install( FILES  kwordpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kword.desktop  DESTINATION ${XDG_APPS_DIR})
