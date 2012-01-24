project(cdrimportfilter)

include_directories(
    ${LIBKORALLE_INCLUDES}
)

set( karboncdrimport_PART_SRCS
    cdrparser.cpp
    cdrimportfilter.cpp
    main.cpp
)

kde4_add_plugin( karboncdrimport ${karboncdrimport_PART_SRCS} )


target_link_libraries( karboncdrimport
    karbonui
    ${LIBKORALLE_LIBRARIES}
)

install(TARGETS karboncdrimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  karbon_cdr_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
