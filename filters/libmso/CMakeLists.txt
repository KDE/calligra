# To hide all the warnings from the generated code
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-sign-compare)
    add_definitions(-Wno-unused-variable)
endif ()

include_directories( ${QT_INCLUDES} ${KDE4_INCLUDES} ${KOODF_INCLUDES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

if(NOT MSVC AND NOT (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "SunPro")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -KPIC")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif()
endif()

add_library(mso STATIC generated/simpleParser.cpp drawstyle.cpp writer.cpp ODrawToOdf.cpp shapes.cpp shapes2.cpp pictures.cpp pole.cpp)
target_link_libraries(mso koodf ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
