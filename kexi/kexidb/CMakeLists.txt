add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44000)

add_subdirectory(drivers)
add_subdirectory(sqlitedump)

include_directories(${CALLIGRADB_INCLUDES} ${KDE4_INCLUDES})

set(kexidb_LIB_SRCS
   dbobjectnamevalidator.cpp
   fieldvalidator.cpp
   alter.cpp
)

kde4_add_library(kexidb SHARED ${kexidb_LIB_SRCS})
target_link_libraries(kexidb kexiutils ${CALLIGRADB_LIBS}
   ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KPARTS_LIBS})

set_target_properties(kexidb PROPERTIES
                      VERSION ${GENERIC_CALLIGRA_LIB_VERSION}
                      SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

if(MSVC)
   # avoid LNK1169 errors
   set_target_properties(kexidb PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
endif(MSVC)

install(TARGETS kexidb ${INSTALL_TARGETS_DEFAULT_ARGS})

if(FALSE) # TODO: install when we move to independent place
   install( FILES
   DESTINATION ${INCLUDE_INSTALL_DIR}/kexidb COMPONENT Devel)
endif(FALSE)
