include_directories( ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${CMAKE_SOURCE_DIR}/kpresenter/part ${Boost_INCLUDE_DIR} )

set(kprvariables_PART_SRCS 
    PresentationVariable.cpp
    PresentationVariableFactory.cpp
    Plugin.cpp
   )

kde4_add_plugin(kprvariables ${kprvariables_PART_SRCS})

target_link_libraries(
    kprvariables 
    kpresenterprivate
    ${KDE4_KDECORE_LIBS}
    ${QT_QTCORE_LIBRARY}
    )

install(TARGETS kprvariables  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kprvariables.desktop DESTINATION ${SERVICES_INSTALL_DIR})
