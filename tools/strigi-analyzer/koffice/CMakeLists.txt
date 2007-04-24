include_directories( ${KOFFICECORE_INCLUDES})

# build the analyzer as a module
kde4_add_library(koffice MODULE kofficethroughanalyzer.cpp)

# link with the required libraries
target_link_libraries(koffice ${STREAMANALYZER_LIBRARY} ${KDE4_KIO_LIBS})

# adjust the name so the module is recognized as a strigi plugin
set_target_properties(koffice PROPERTIES
    PREFIX strigita_)

# install the module in the right directory so it is picked up
install(TARGETS koffice LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strigi)

