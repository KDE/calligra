include_directories( 
  ${CMAKE_SOURCE_DIR}/kexi 
  ${KDE4_INCLUDES}
)

set(kexiutils_LIB_SRCS identifier.cpp validator.cpp utils.cpp debuggui.cpp 
  styleproxy.cpp longlongvalidator.cpp InternalPropertyMap.cpp
  SmallToolButton.cpp FlowLayout.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/transliteration_table.cpp)

add_definitions( -DMAKE_KEXIUTILS_LIB )
kde4_add_library(kexiutils SHARED ${kexiutils_LIB_SRCS})
target_link_libraries(kexiutils  
  ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS} )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/transliteration_table.cpp
  COMMAND ${BZIP2_EXECUTABLE} ARGS -dc -k ${CMAKE_CURRENT_SOURCE_DIR}/transliteration_table.cpp.bz2 > ${CMAKE_CURRENT_BINARY_DIR}/transliteration_table.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/transliteration_table.cpp.bz2
)

macro_additional_clean_files( ${CMAKE_CURRENT_BINARY_DIR}/transliteration_table.cpp )

set_target_properties(kexiutils PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kexiutils  DESTINATION ${LIB_INSTALL_DIR})

install( FILES  tristate.h identifier.h validator.h utils.h kexiutils_export.h kexiutils_global.h
  styleproxy.h longlongvalidator.h InternalPropertyMap.h 
  SmallToolButton.h FlowLayout.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/kexiutils)

