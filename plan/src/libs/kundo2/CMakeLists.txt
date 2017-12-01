include_directories(${PLANKUNDO2_INCLUDES})

set(kundo2_LIB_SRCS
	kundo2stack.cpp
	kundo2group.cpp
	kundo2view.cpp
	kundo2model.cpp
	kundo2magicstring.cpp
	kundo2commandextradata.cpp
)

add_library(plankundo2 SHARED ${kundo2_LIB_SRCS})
generate_export_header(plankundo2 BASE_NAME kundo2)

target_link_libraries(plankundo2
    PUBLIC
        KF5::I18n
        KF5::XmlGui
        Qt5::Core
        Qt5::Widgets
    PRIVATE
        KF5::ConfigWidgets
)

set_target_properties(plankundo2 PROPERTIES
    VERSION ${GENERIC_PLAN_LIB_VERSION} SOVERSION ${GENERIC_PLAN_LIB_SOVERSION}
)
install(TARGETS plankundo2 ${INSTALL_TARGETS_DEFAULT_ARGS})
