include_directories(
    ${KOWIDGETS_INCLUDES}
)

set(calligradocinfopropspage_PART_SRCS KoDocInfoPropsFactory.cpp)

add_library(calligradocinfopropspage MODULE ${calligradocinfopropspage_PART_SRCS})

target_link_libraries(calligradocinfopropspage  KF5::KIOWidgets kowidgets)

install(TARGETS calligradocinfopropspage DESTINATION ${KDE_INSTALL_PLUGINDIR})

install(FILES calligradocinfopropspage.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
