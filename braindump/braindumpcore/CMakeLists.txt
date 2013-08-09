set(braindumpcore_LIB_SRCS
  State.cpp
  StateCategory.cpp
  StatesRegistry.cpp
)

kde4_add_library(braindumpcore SHARED ${braindumpcore_LIB_SRCS})
target_link_libraries(braindumpcore ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} kofake)

target_link_libraries(braindumpcore LINK_INTERFACE_LIBRARIES ${QT_QTCORE_LIBRARY})

set_target_properties(braindumpcore PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION})

install(TARGETS braindumpcore ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP )
