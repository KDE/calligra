include_directories(${KUNDO2_INCLUDES})

set(kundo2_LIB_SRCS
	kundo2stack.cpp
	kundo2group.cpp
	kundo2view.cpp
	kundo2model.cpp
	kundo2magicstring.cpp
	kundo2commandextradata.cpp
)

add_library(kundo2 SHARED ${kundo2_LIB_SRCS})
generate_export_header(kundo2 BASE_NAME kundo2)

target_link_libraries(kundo2
    PUBLIC
        KF6::I18n
        KF6::XmlGui
        Qt6::Core
        Qt6::Widgets
    PRIVATE
        KF6::ConfigWidgets
)

set_target_properties(kundo2 PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS kundo2 ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if (SHOULD_BUILD_DEVEL_HEADERS)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kundo2_export.h
    kundo2stack.h
    kundo2qstack.h
    kundo2group.h
    kundo2view.h
    kundo2model.h
    kundo2command.h
    kundo2magicstring.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel
)

endif()
