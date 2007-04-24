include_directories( ${KOFFICECORE_INCLUDES})
# build the analyzer as a module
kde4_add_library(abiword MODULE abiwordthroughanalyzer.cpp)

# link with the required libraries
target_link_libraries(abiword ${STREAMANALYZER_LIBRARY} ${KDE4_KIO_LIBS})

# adjust the name so the module is recognized as a strigi plugin
set_target_properties(abiword PROPERTIES
    PREFIX strigita_)

# install the module in the right directory so it is picked up
install(TARGETS abiword LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strigi)


