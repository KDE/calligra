include_directories(
    ${WPD_INCLUDE_DIR}
    ${LIBWPG_INCLUDE_DIR}
)

if(NOT MSVC AND NOT (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "SunPro")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -KPIC")
    else("${CMAKE_CXX_COMPILER_ID}" MATCHES "SunPro")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif("${CMAKE_CXX_COMPILER_ID}" MATCHES "SunPro")
endif(NOT MSVC AND NOT (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))

kde4_add_library(writerperfect STATIC
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
