

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core   )


########### next target ###############

set(kexirelationsview_LIB_SRCS 
   KexiRelationsScrollArea.cpp
   KexiRelationsConnection.cpp
   KexiRelationsTableContainer.cpp
   KexiRelationsTableContainer_p.cpp
   KexiRelationsView.cpp
)


add_library(kexirelationsview SHARED ${kexirelationsview_LIB_SRCS})

target_link_libraries(
    kexirelationsview
    kexicore
    kexiextendedwidgets
    kexidb
    kexiutils
    ${CALLIGRADB_LIBS}
    
    
    Qt5::Core
    Qt5::Gui
    )

set_target_properties(kexirelationsview PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS kexirelationsview  ${INSTALL_TARGETS_DEFAULT_ARGS})
