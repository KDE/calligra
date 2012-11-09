include_directories( ${VECTORIMAGE_INCLUDES} ${KDE4_INCLUDES} )

# The EMF library is directly under this directory.
# When it becomes an entity on its own we have to create
# some cmake code here to find it.

# Duplicate from filters/CMakeLists.txt
#set(WMF_INCLUDES ${CMAKE_SOURCE_DIR}/filters/libkowmf)
# SET(WMF_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/libwmf)

include_directories(${KOMAIN_INCLUDES})

set(vectorimage_LIB_SRCS    
    libemf/EmfRecords.cpp
    libemf/EmfObjects.cpp
    libemf/EmfHeader.cpp
    libemf/BitmapHeader.cpp
    libemf/Bitmap.cpp
    libemf/EmfParser.cpp
    libemf/EmfOutput.cpp
    libemf/EmfOutputDebugStrategy.cpp
    libemf/EmfOutputPainterStrategy.cpp

    libsvm/SvmStructs.cpp
    libsvm/SvmGraphicsContext.cpp
    libsvm/SvmParser.cpp
    libsvm/SvmPainterBackend.cpp
)

kde4_add_library(vectorimage SHARED ${vectorimage_LIB_SRCS})

target_link_libraries(vectorimage ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})
target_link_libraries(vectorimage LINK_INTERFACE_LIBRARIES ${KDE4_KDECORE_LIBS})

set_target_properties(vectorimage PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS vectorimage ${INSTALL_TARGETS_DEFAULT_ARGS} )

