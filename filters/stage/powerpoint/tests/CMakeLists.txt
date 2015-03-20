include_directories(${QT_INCLUDES} ${KOMAIN_INCLUDES} ${KOODF_INCLUDES} .. ../../../libmso)

set(TestPPT_SRCS TestPPT.cpp )
kde4_add_unit_test(TestPPT TESTNAME filters-powerpoint-PPT ${TestPPT_SRCS})
target_link_libraries(TestPPT koodf ${QT_QTTEST_LIBRARY} ppttoodplib komain)
