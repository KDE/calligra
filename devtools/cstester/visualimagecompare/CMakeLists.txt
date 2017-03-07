if (POPPLER_FOUND)
    add_definitions(-DHAS_POPPLER)

include_directories(
    
    ${POPPLER_INCLUDE_DIR}
)

set(visualimagecompare_SRCS
    CSTCompareView.h
    CompareView.h
    CSTCompareView.cpp
    CompareView.cpp
    visualimagecompare.cpp)

add_executable(visualimagecompare ${visualimagecompare_SRCS})

target_link_libraries(visualimagecompare ${POPPLER_LIBRARY} Qt5::Core Qt5::Widgets)

install(TARGETS visualimagecompare ${INSTALL_TARGETS_DEFAULT_ARGS})

endif ()
