include_directories(
    
)

set(updateautocorrection_SRCS
    updateautocorrection.cpp
    Importabstractautocorrection.cpp
    importkmailautocorrection.cpp
    importlibreofficeautocorrection.cpp
    Autocorrection.cpp
)

add_executable(updateautocorrection ${updateautocorrection_SRCS})

target_link_libraries(updateautocorrection Qt6::Core Qt::Xml KF6::Archive)

set(AUTOCORRECTIONDATA "${CMAKE_SOURCE_DIR}/plugins/textediting/autocorrection/data")
message(STATUS "${CMAKE_SOURCE_DIR} ${AUTOCORRECTIONDATA}")
configure_file(ACConfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/ACConfig.h)

#install(TARGETS updateautocorrection ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

