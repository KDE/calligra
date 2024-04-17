if (Poppler_Qt6_FOUND)
    add_definitions(-DHAS_POPPLER)

set(visualimagecompare_SRCS
    CSTCompareView.h
    CompareView.h
    CSTCompareView.cpp
    CompareView.cpp
    visualimagecompare.cpp)

add_executable(visualimagecompare ${visualimagecompare_SRCS})

target_link_libraries(visualimagecompare Poppler::Qt6 Qt6::Core Qt6::Widgets)

install(TARGETS visualimagecompare ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

endif ()
