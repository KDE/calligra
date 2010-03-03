include_directories( ${QT_INCLUDES}  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fPIC")
add_library(mso STATIC generated/simpleParser.cpp)
