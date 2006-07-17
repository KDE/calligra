
if(KDE4_BUILD_TESTS)

add_definitions(-DCOMPILING_TESTS)

include_directories( ${KOFFICECORE_INCLUDES} ${FLAKE_INCLUDES} ${KOTEXT_INCLUDES} ${KDE4_INCLUDES}
    ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/kword/part)

########### next target ###############

set(TestPageManager_test_SRCS TestPageManager.cpp)
kde4_automoc(${TestPageManager_test_SRCS})
kde4_add_executable(TestPageManager RUN_UNINSTALLED ${TestPageManager_test_SRCS})
target_link_libraries(TestPageManager kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

endif(KDE4_BUILD_TESTS)
