include_directories(${QT_INCLUDES} ${KOMAIN_INCLUDES} ${KOODF_INCLUDES} .. ../../../libmso)

ecm_add_test( TestPPT.cpp
    TEST_NAME "FormulaParser"
    NAME_PREFIX "filter-ppt2odp-PPT"
    LINK_LIBRARIES koodf ppttoodplib komain Qt5::Test
)
