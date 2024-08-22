include_directories(
    ../../../outputhelper/
    ${LIBODFGEN_INCLUDE_DIRS}
    ${LIBETONYEK_INCLUDE_DIRS}
    ${KOMAIN_INCLUDES}
)

set(key2odp_PART_SRCS
    KeyImport.cpp
    ../../../outputhelper/StringDocumentHandler.cxx
    ../../../outputhelper/FemtoZip.cxx
    ../../../outputhelper/OutputFileHelper.cxx
)

add_library(calligra_filter_key2odp MODULE ${key2odp_PART_SRCS})

target_link_libraries(calligra_filter_key2odp PRIVATE
    komain
    LibRevenge::LibRevenge
    ${LIBODFGEN_LIBRARIES}
    ${LIBETONYEK_LIBRARIES}
)

install(TARGETS calligra_filter_key2odp DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

