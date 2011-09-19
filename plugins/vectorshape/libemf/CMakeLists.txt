

include_directories( ${KOMAIN_INCLUDES} )

add_subdirectory(demo)


########### library target ###############

set(libemf_LIB_SRCS 
     EmfDeviceContext.cpp
     EmfRecords.cpp
     EmfObjects.cpp
     EmfHeader.cpp
     BitmapHeader.cpp
     EmfParser.cpp
     EmfplusDeviceContext.cpp
     EmfplusParser.cpp
     EmfAbstractBackend.cpp
     EmfDebugBackend.cpp
     EmfPainterBackend.cpp
)

#kde4_add_library(libemf SHARED ${libemf_LIB_SRCS})

#target_link_libraries(libemf  ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} )

#set_target_properties(libemf PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )


########### install files ###############

#install(TARGETS libemf  ${INSTALL_TARGETS_DEFAULT_ARGS})

#install(
#    FILES
#    EmfEnums.h
#    EmfDeviceContext.h
#    EmfRecords.h
#    EmfObjects.h
#    EmfHeader.h
#    EmfplusEnums.h
#    EmfplusDeviceContext.h
#    EmfplusDeviceParser.h
#    Bitmap.h
#    EmfParser.h
#    EmfAbstractBackend.h
#    EmfDebugBackend.h
#    EmfPainterBackend.h
#    emf_export.h
#    DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
#)
