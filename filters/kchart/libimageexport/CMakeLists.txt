

include_directories(
	${CMAKE_SOURCE_DIR}/kchart 
	${CMAKE_SOURCE_DIR}/kchart/kdchart 
	${CMAKE_SOURCE_DIR}/interfaces 
	${CMAKE_SOURCE_DIR}/filters/libdialogfilter 
	${KDE4_INCLUDE_DIR} 
	${QT_INCLUDES}  )


########### next target ###############

set(kchartimageexport_LIB_SRCS imageexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(kchartimageexport ${kchartimageexport_LIB_SRCS})

kde4_add_library(kchartimageexport SHARED ${kchartimageexport_LIB_SRCS})

target_link_libraries(kchartimageexport  ${KDE4_KDECORE_LIBS} kchartcommon)

set_target_properties(kchartimageexport PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kchartimageexport  DESTINATION ${LIB_INSTALL_DIR})


