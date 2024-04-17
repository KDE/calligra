# To hide all the warnings from the generated code
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-sign-compare)
    add_definitions(-Wno-unused-variable)
endif ()

include_directories(
    ${KOODF_INCLUDES}
    ${ZLIB_INCLUDE_DIR}
)

kde_enable_exceptions()

if(NOT MSVC AND NOT (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

add_library(mso STATIC
    generated/simpleParser.cpp
    drawstyle.cpp
    writer.cpp
    ODrawToOdf.cpp
    shapes.cpp
    shapes2.cpp
    pictures.cpp
    pole.cpp
    MsoUtils.cpp
    NumberFormatParser.cpp
)
target_link_libraries(mso
    PUBLIC
        koodf
    PRIVATE
        Qt6::Gui
        ${ZLIB_LIBRARIES}
)
