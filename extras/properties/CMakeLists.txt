include_directories(
    ${KOWIDGETS_INCLUDES}
)

set(calligradocinfopropspage_PART_SRCS KoDocInfoPropsFactory.cpp)

kde4_add_plugin(calligradocinfopropspage ${calligradocinfopropspage_PART_SRCS})

target_link_libraries(calligradocinfopropspage ${KDE4_KFILE_LIBS} kowidgets)

install(TARGETS calligradocinfopropspage DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES calligradocinfopropspage.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
