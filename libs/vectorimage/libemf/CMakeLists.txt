

include_directories( ${KOMAIN_INCLUDES} )

## add_subdirectory(demo)


########### library target ###############

set(libemf_LIB_SRCS 
     EmfRecords.cpp
     EmfObjects.cpp
     EmfHeader.cpp
     BitmapHeader.cpp
     EmfParser.cpp
     EmfOutput.cpp
     EmfOutputDebugStrategy.cpp
     EmfOutputPainterStrategy.cpp
)

#add_library(libemf SHARED ${libemf_LIB_SRCS})

#target_link_libraries(libemf   Qt6::Gui )

#set_target_properties(libemf PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION} )


########### install files ###############

#install(TARGETS libemf  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

#install(
#    FILES
#    EmfEnums.h
#    EmfRecords.h
#    EmfObjects.h
#    EmfHeader.h
#    Bitmap.h
#    EmfParser.h
#    EmfOutput.h
#    EmfOutputDebugStrategy.h
#    EmfOutputPainterStrategy.h
#    emf_export.h
#    DESTINATION ${KDE_INSTALL_INCLUDEDIR} COMPONENT Devel
#)
