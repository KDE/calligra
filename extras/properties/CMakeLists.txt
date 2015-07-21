include_directories(
    ${KOWIDGETS_INCLUDES}
)

set(calligradocinfopropspage_PART_SRCS KoDocInfoPropsFactory.cpp)

add_library(calligradocinfopropspage MODULE ${calligradocinfopropspage_PART_SRCS})

target_link_libraries(calligradocinfopropspage  KF5::KIOWidgets kowidgets)

install(TARGETS calligradocinfopropspage DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES calligradocinfopropspage.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
