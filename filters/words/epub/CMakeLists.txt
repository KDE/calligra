########### next target ###############

set(WMF_INCLUDES ${CMAKE_SOURCE_DIR}/filters/libkowmf/)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libsvm/
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/
    ${WMF_INCLUDES}
    ${KOMAIN_INCLUDES}
    ${FLAKE_INCLUDES}
)
set(exportepub2_PART_SRCS
    exportepub2.cpp
    OdfParser.cpp
    OdtHtmlConverter.cpp

    EpubFile.cpp
    FileCollector.cpp
    
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libsvm/SvmStructs.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libsvm/SvmGraphicsContext.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libsvm/SvmParser.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libsvm/SvmPainterBackend.cpp
    
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfRecords.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfObjects.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfHeader.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/BitmapHeader.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/Bitmap.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfParser.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfOutput.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfOutputDebugStrategy.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfOutputPainterStrategy.cpp
    
    ${CMAKE_SOURCE_DIR}/filters/libkowmf/WmfPainterBackend.cpp
    ${CMAKE_SOURCE_DIR}/filters/libkowmf/WmfAbstractBackend.cpp
    ${CMAKE_SOURCE_DIR}/filters/libkowmf/WmfParser.cpp
    ${CMAKE_SOURCE_DIR}/filters/libkowmf/WmfDeviceContext.cpp
    ${CMAKE_SOURCE_DIR}/filters/libkowmf/WmfStack.cpp
)

set(exporthtml_PART_SRCS
    exporthtml.cpp
    OdfParser.cpp
    OdtHtmlConverter.cpp

    HtmlFile.cpp
    FileCollector.cpp
    
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libsvm/SvmStructs.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libsvm/SvmGraphicsContext.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libsvm/SvmParser.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libsvm/SvmPainterBackend.cpp
    
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfRecords.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfObjects.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfHeader.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/BitmapHeader.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/Bitmap.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfParser.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfOutput.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfOutputDebugStrategy.cpp
    ${CMAKE_SOURCE_DIR}/plugins/vectorshape/libemf/EmfOutputPainterStrategy.cpp
    
    ${CMAKE_SOURCE_DIR}/filters/libkowmf/WmfPainterBackend.cpp
    ${CMAKE_SOURCE_DIR}/filters/libkowmf/WmfAbstractBackend.cpp
    ${CMAKE_SOURCE_DIR}/filters/libkowmf/WmfParser.cpp
    ${CMAKE_SOURCE_DIR}/filters/libkowmf/WmfDeviceContext.cpp
    ${CMAKE_SOURCE_DIR}/filters/libkowmf/WmfStack.cpp
)


kde4_add_plugin(exportepub2 ${exportepub2_PART_SRCS})
kde4_add_plugin(exporthtml ${exporthtml_PART_SRCS})

target_link_libraries(exportepub2 komain)
target_link_libraries(exporthtml komain)


 ########### install files ###############

install(TARGETS exportepub2  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  words_epub2_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

install(TARGETS exporthtml  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  words_html_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
