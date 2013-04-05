set(koplugin_LIB_SRCS
	KoPluginLoader.cpp
)

kde4_add_library(koplugin SHARED ${koplugin_LIB_SRCS})

target_link_libraries(koplugin Qt5::Core kofake)
target_link_libraries(koplugin LINK_INTERFACE_LIBRARIES Qt5::Core kofake)

set_target_properties(koplugin PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS koplugin ${INSTALL_TARGETS_DEFAULT_ARGS} )

