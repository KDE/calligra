message(STATUS "Adding plan_rng2cpp executable")

add_executable(plan_rng2cpp rng2cpp.cpp)
target_link_libraries(plan_rng2cpp Qt5::Core Qt5::Xml)
