

include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${KDE4_INCLUDES}  )


########### next target ###############

set(kpresenterimageexport_LIB_SRCS imageexport.cpp )

kde4_automoc(${kpresenterimageexport_LIB_SRCS})

kde4_add_library(kpresenterimageexport SHARED ${kpresenterimageexport_LIB_SRCS})

target_link_libraries(kpresenterimageexport  ${KDE4_KDECORE_LIBS} kpresenterprivate )

set_target_properties(kpresenterimageexport PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kpresenterimageexport  DESTINATION ${LIB_INSTALL_DIR})
