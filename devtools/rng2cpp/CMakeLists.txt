find_package(Qt4 REQUIRED QtCore QtXml)
include(${QT_USE_FILE})
add_executable(rng2cpp rng2cpp.cpp)
target_link_libraries(rng2cpp ${QT_LIBRARIES})
