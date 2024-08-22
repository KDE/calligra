set(braindumpcore_LIB_SRCS
  State.cpp
  StateCategory.cpp
  StatesRegistry.cpp
)

add_library(braindumpcore SHARED ${braindumpcore_LIB_SRCS})
generate_export_header(braindumpcore)
target_link_libraries(braindumpcore
    PUBLIC
        Qt6::Svg
        Qt6::Core
    PRIVATE
        KF6::I18n
        Qt6::Xml
)

set_target_properties(braindumpcore PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION})

install(TARGETS braindumpcore ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP )
