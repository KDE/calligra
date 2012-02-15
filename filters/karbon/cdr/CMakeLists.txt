project(cdrimportfilter)

set( cheadergenerator_SRCS
    generator/formatdocument.cpp
    generator/formatparser.cpp
    generator/cheadergenerator.cpp
    generator/main.cpp
)

kde4_add_executable(cheadergenerator  ${cheadergenerator_SRCS})
target_link_libraries(cheadergenerator
    ${QT_QTCORE_LIBRARY}
    ${QT_QTXML_LIBRARY}
)

add_custom_command(
    OUTPUT cdr4structs.h
    COMMAND cheadergenerator
    ARGS cdr4structs.h "${CMAKE_CURRENT_SOURCE_DIR}/cdr4structs.format"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/cdr4structs.format" cheadergenerator
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

include_directories(
    ${LIBKORALLE_INCLUDES}
)

set( karboncdrimport_PART_SRCS
    cdr4structs.h # find a better way to add to dep tree
    cdrdocument.cpp
    cdrparser.cpp
    cdrodgwriter.cpp
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
