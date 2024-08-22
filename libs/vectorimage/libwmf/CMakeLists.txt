

include_directories( ${KOMAIN_INCLUDES} )


########### next target ###############

#set(kwmf_LIB_SRCS kwmf.cc qwmf.cc )
#
#
#add_library(kwmf SHARED ${kwmf_LIB_SRCS})
#
#target_link_libraries(kwmf   Qt6::Gui)
#
#set_target_properties(kwmf PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION} )
#install(TARGETS kwmf  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(libwmf_LIB_SRCS 
   WmfStack.cpp
   WmfDeviceContext.cpp
   WmfParser.cpp 
   WmfAbstractBackend.cpp 
   WmfPainterBackend.cpp

   WmfWriter.cpp
 )

add_library(libwmf SHARED ${libwmf_LIB_SRCS})

target_link_libraries(libwmf   Qt6::Gui)

set_target_properties(libwmf PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION} )
install(TARGETS libwmf  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

