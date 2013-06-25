add_subdirectory(wv2)

INCLUDE_DIRECTORIES(
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
   ../../sheets/xlsx/NumberFormatParser.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

kde4_add_plugin(calligra_filter_doc2odt ${doc2odt_PART_SRCS})

#TODO cause of NumberFormatParser we depend on msooxml. Fix that!
#target_link_libraries(calligra_filter_doc2odt kowv2 komain mso ${ZLIB_LIBRARIES})
target_link_libraries(calligra_filter_doc2odt kowv2 komain mso msooxml ${ZLIB_LIBRARIES})

install(TARGETS calligra_filter_doc2odt  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES  words_msword-odf_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES  words_msword_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
