add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44024)

include_directories( 
  ${CMAKE_SOURCE_DIR}/kexi 
  ${KDE4_INCLUDES}
)

set(kexiutils_LIB_SRCS identifier.cpp validator.cpp utils.cpp debuggui.cpp 
  styleproxy.cpp longlongvalidator.cpp InternalPropertyMap.cpp
  SmallToolButton.cpp FlowLayout.cpp
  transliteration_table.cpp)

kde4_add_library(kexiutils SHARED ${kexiutils_LIB_SRCS})
target_link_libraries(kexiutils  ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS} )
target_link_libraries(kexiutils  LINK_INTERFACE_LIBRARIES ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS})

set_target_properties(kexiutils PROPERTIES
    VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION}
)

install(TARGETS kexiutils  ${INSTALL_TARGETS_DEFAULT_ARGS})

if(FALSE) # TODO: install when we move to independent place
    install( FILES  tristate.h identifier.h validator.h utils.h kexiutils_export.h kexiutils_global.h
    styleproxy.h longlongvalidator.h InternalPropertyMap.h 
    SmallToolButton.h FlowLayout.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/kexiutils COMPONENT Devel)
endif(FALSE)
