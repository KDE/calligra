include_directories(${QT_INCLUDES} ${KOMAIN_INCLUDES} ${KOODF_INCLUDES} .. ../../../libmso)

ecm_add_test( TestPPT.cpp
    TEST_NAME "PPT"
    NAME_PREFIX "filter-ppt2odp-"
    LINK_LIBRARIES koodf ppttoodplib komain Qt5::Test
)
