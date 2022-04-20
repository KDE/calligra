if (Poppler_Qt5_FOUND)
    add_definitions(-DHAS_POPPLER)

set(visualimagecompare_SRCS
    CSTCompareView.h
    CompareView.h
    CSTCompareView.cpp
    CompareView.cpp
    visualimagecompare.cpp)

add_executable(visualimagecompare ${visualimagecompare_SRCS})

target_link_libraries(visualimagecompare Poppler::Qt5 Qt5::Core Qt5::Widgets)

install(TARGETS visualimagecompare ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

endif ()
