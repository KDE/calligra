set(braindumpcore_LIB_SRCS
  State.cpp
  StateCategory.cpp
  StatesRegistry.cpp
)

add_library(braindumpcore SHARED ${braindumpcore_LIB_SRCS})
generate_export_header(braindumpcore)
target_link_libraries(braindumpcore
    PUBLIC
        Qt5::Svg
        Qt5::Core
    PRIVATE
        KF5::I18n
        Qt5::Xml
)

set_target_properties(braindumpcore PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION})

install(TARGETS braindumpcore ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP )
