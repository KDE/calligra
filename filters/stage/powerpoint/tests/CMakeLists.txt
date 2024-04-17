include_directories(
    ${KOMAIN_INCLUDES}
    ${KOODF_INCLUDES}
    ..
    ../../../libmso
)

ecm_add_test( TestPPT.cpp
    TEST_NAME "PPT"
    NAME_PREFIX "filter-ppt2odp-"
    LINK_LIBRARIES koodf ppttoodplib komain Qt6::Test
)
