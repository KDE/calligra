include_directories( ${KOMAIN_INCLUDES} ${KOODF_INCLUDES} .. ../../../libmso)

set(TestPPT_SRCS TestPPT.cpp )
add_executable(TestPPTNAME filters-powerpoint-PPT ${TestPPT_SRCS})
add_test(TestPPT TestPPT)
ecm_mark_as_test(TestPPT)
target_link_libraries(TestPPT koodf Qt5::Test ppttoodplib komain)
