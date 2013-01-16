macro_optional_find_package(Poppler)

if (POPPLER_FOUND)
    add_definitions(-DHAS_POPPLER)

include_directories(
    ${QT_INCLUDES}
    ${POPPLER_INCLUDE_DIR}
)

set(visualimagecompare_SRCS
    CSTCompareView.h
    CompareView.h
    CSTCompareView.cpp
    CompareView.cpp
    visualimagecompare.cpp)

kde4_add_executable(visualimagecompare ${visualimagecompare_SRCS})

target_link_libraries(visualimagecompare ${POPPLER_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS visualimagecompare ${INSTALL_TARGETS_DEFAULT_ARGS})

endif (POPPLER_FOUND)
