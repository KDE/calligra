set(braindumpcore_LIB_SRCS
  State.cpp
  StateCategory.cpp
  StatesRegistry.cpp
)

add_library(braindumpcore SHARED ${braindumpcore_LIB_SRCS})
generate_export_header(braindumpcore)
target_link_libraries(braindumpcore Qt5::Core Qt5::Xml Qt5::Svg komain)

target_link_libraries(braindumpcore LINK_INTERFACE_LIBRARIES Qt5::Core)

set_target_properties(braindumpcore PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION})

install(TARGETS braindumpcore ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP )
