set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${PLANMAIN_INCLUDES} )

add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")

# call: planmain_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(PLANMAIN_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "Plan${_TEST_NAME}"
        NAME_PREFIX "plan-main-"
    )
endmacro()

########### next target ###############

set(priorityqueue_test_SRCS priorityqueue_test.cpp ../MainDebug.cpp)
planmain_add_unit_test(priorityQueueTest ${priorityqueue_test_SRCS}  LINK_LIBRARIES KF5::I18n Qt5::Test)

########### next target ###############

planmain_add_unit_test(filterchainTest filterchain_test.cpp  LINK_LIBRARIES planmain)

########### next target ###############

planmain_add_unit_test(filterGraphTest filter_graph.cpp  LINK_LIBRARIES planmain)


