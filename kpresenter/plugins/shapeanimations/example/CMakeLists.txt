include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  )

set(kpr_shapeanimation_example_PART_SRCS Plugin.cpp )

kde4_add_plugin(kpr_shapeanimation_example ${kpr_shapeanimation_example_PART_SRCS})

target_link_libraries(
    kpr_shapeanimation_example 
    calligrastageprivate
    ${KDE4_KDECORE_LIBS}
    ${QT_QTCORE_LIBRARY}
    )

install(TARGETS kpr_shapeanimation_example  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kpr_shapeanimation_example.desktop DESTINATION ${SERVICES_INSTALL_DIR})


