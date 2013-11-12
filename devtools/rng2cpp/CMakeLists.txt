include_directories(${QT_INCLUDES})
add_executable(rng2cpp rng2cpp.cpp)
target_link_libraries(rng2cpp ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY})
