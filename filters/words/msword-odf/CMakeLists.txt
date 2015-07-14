add_subdirectory(wv2)

include_directories(
    ${ICONV_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR}
    ../../libmso
    ../../libmsooxml
    ../../sheets/xlsx/
)

set(doc2odt_PART_SRCS
   mswordodfimport.cpp
   document.cpp
   conversion.cpp
   texthandler.cpp
   paragraph.cpp
   tablehandler.cpp
   graphicshandler.cpp
   drawclient.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

kde4_add_plugin(calligra_filter_doc2odt ${doc2odt_PART_SRCS})

#TODO cause of NumberFormatParser we depend on komsooxml. Fix that!
#target_link_libraries(calligra_filter_doc2odt kowv2 komain mso ${ZLIB_LIBRARIES})
target_link_libraries(calligra_filter_doc2odt kowv2 komain mso komsooxml ${ZLIB_LIBRARIES})

install(TARGETS calligra_filter_doc2odt  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  calligra_filter_doc2odt.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES  words_msword_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif()

if(SHOULD_BUILD_OKULAR_GENERATOR_DOC)
    install( FILES libokularGenerator_doc_calligra.desktop okularDoc_calligra.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
    install( PROGRAMS okularApplication_doc_calligra.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
endif()
