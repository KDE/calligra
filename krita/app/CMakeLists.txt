include_directories(${KOMVC_INCLUDES})
add_definitions(${KDE4_ENABLE_EXCEPTIONS})

set( qrita_SRCS
    kis_application.cpp
    kis_document_factory.cpp
    kis_main.cpp
    kis_view_factory.cpp
    kis_mdi_mainwindow.cpp
)

kde4_add_app_icon(qrita "${CMAKE_CURRENT_SOURCE_DIR}/../pics/app/*.png")
kde4_add_executable(qrita ${qrita_SRCS})

target_link_libraries( qrita kritaui komvc)

install( TARGETS qrita ${INSTALL_TARGETS_DEFAULT_ARGS} )
