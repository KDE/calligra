set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${KOMAIN_INCLUDES} )

add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")

# call: komain_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(KOMAIN_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-komain-"
    )
endmacro()

########### next target ###############
if (NOT WIN32)
  # This does not link on windows, due to the ../MainDebug.cpp
  set(priorityqueue_test_SRCS priorityqueue_test.cpp ../MainDebug.cpp)
  komain_add_unit_test(priorityQueueTest ${priorityqueue_test_SRCS}  LINK_LIBRARIES KF6::I18n Qt6::Test)
endif()
########### next target ###############

komain_add_unit_test(filterchainTest filterchain_test.cpp  LINK_LIBRARIES komain)

########### next target ###############

komain_add_unit_test(filterGraphTest filter_graph.cpp  LINK_LIBRARIES komain)

########### next target ###############

set(rtreetestapp_SRCS rtreetestapp.cpp Tool.cpp )
add_executable(rtreetestapp ${rtreetestapp_SRCS})
ecm_mark_as_test(rtreetestapp)
target_link_libraries(rtreetestapp komain Qt6::Test )

########### next target ###############

komain_add_unit_test(DocumentStructureTest documentstructuretest.cpp  LINK_LIBRARIES komain Qt6::Test)

########### next target ###############

komain_add_unit_test(testfindmatch testfindmatch.cpp  LINK_LIBRARIES komain Qt6::Test)

