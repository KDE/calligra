include_directories(
    ${QT_INCLUDES}
)

set(visualimagecompare_SRCS
    CSTCompareView.h
    CompareView.h
    CSTCompareView.cpp
    CompareView.cpp
    visualimagecompare.cpp)

kde4_add_executable(visualimagecompare ${visualimagecompare_SRCS})

target_link_libraries(visualimagecompare ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS visualimagecompare ${INSTALL_TARGETS_DEFAULT_ARGS})

