add_subdirectory(wv2)

include_directories(
    ${ICONV_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR}
    ../../libmso
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

kde_enable_exceptions()

add_library(calligra_filter_doc2odt MODULE ${doc2odt_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_doc2odt calligra_filter_doc2odt.desktop)

target_link_libraries(calligra_filter_doc2odt kowv2 komain mso ${ZLIB_LIBRARIES} KF5::KDELibs4Support)

install(TARGETS calligra_filter_doc2odt  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES  words_msword_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif()

if(SHOULD_BUILD_OKULAR_GENERATOR_DOC)
    install( FILES libokularGenerator_doc_calligra.desktop okularDoc_calligra.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
    install( PROGRAMS okularApplication_doc_calligra.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
endif()
