include_directories( ${VECTORIMAGE_INCLUDES}  )

set(vectorimage_LIB_SRCS    
    libemf/EmfRecords.cpp
    libemf/EmfObjects.cpp
    libemf/EmfHeader.cpp
    libemf/BitmapHeader.cpp
    libemf/Bitmap.cpp
    libemf/EmfParser.cpp
    libemf/EmfOutput.cpp
    libemf/EmfOutputPainterStrategy.cpp

    libsvm/SvmStructs.cpp
    libsvm/SvmGraphicsContext.cpp
    libsvm/SvmParser.cpp
    libsvm/SvmPainterBackend.cpp
    
    libwmf/WmfStack.cpp
    libwmf/WmfDeviceContext.cpp
    libwmf/WmfParser.cpp 
    libwmf/WmfAbstractBackend.cpp 
    libwmf/WmfPainterBackend.cpp
    libwmf/WmfWriter.cpp

    VectorImageDebug.cpp
)

option(LIBEMF_DEBUG "Enable the kovectorimage emf debug strategy. Usually you'll want to leave this development support option off." OFF)
if(LIBEMF_DEBUG)
set(vectorimage_LIB_SRCS
    ${vectorimage_LIB_SRCS}
    libemf/EmfOutputDebugStrategy.cpp
    )
endif()

add_library(kovectorimage SHARED ${vectorimage_LIB_SRCS})
generate_export_header(kovectorimage BASE_NAME kovectorimage)

target_link_libraries(kovectorimage
    PUBLIC
        Qt6::Gui
    PRIVATE
        Qt6::PrintSupport
)

set_target_properties(kovectorimage PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
if(LIBEMF_DEBUG)
    target_compile_definitions(kovectorimage PUBLIC -DLIBEMF_DEBUG=true)
endif()
install(TARGETS kovectorimage ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
