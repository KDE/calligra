include_directories(
    ${WPD_INCLUDE_DIR}
    ${LIBWPG_INCLUDE_DIR}
)
add_library(writerperfect SHARED
            DiskDocumentHandler.cxx
            DocumentElement.cxx
            FemtoZip.cxx
            FontStyle.cxx
            InternalHandler.cxx
            ListStyle.cxx
            OdgGenerator.cxx
            OdtGenerator.cxx
            OutputFileHelper.cxx
            PageSpan.cxx
            SectionStyle.cxx
            StdOutHandler.cxx
            TableStyle.cxx
            TextRunStyle.cxx
)
target_link_libraries(writerperfect ${WPD_LIBRARIES} ${LIBWPG_LIBRARIES})

set_target_properties(writerperfect PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS writerperfect ${INSTALL_TARGETS_DEFAULT_ARGS})
