set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_DIRECTORY} )

include_directories( ${CMAKE_SOURCE_DIR}/plugins/chartshape
                     #${CMAKE_SOURCE_DIR}/interfaces
                     #${CMAKE_SOURCE_DIR}/kchart/shape/dialogs
                     ${KOMAIN_INCLUDES} )

# call: chartshape_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(CHARTSHAPE_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "shapes-chart-"
    )
endmacro()

chartshape_add_unit_test(TestChartLayout
    TestChartLayout.cpp
    LINK_LIBRARIES chartshapecore Qt6::Test
)

chartshape_add_unit_test(TestProxyModel
    TestProxyModel.cpp
    LINK_LIBRARIES chartshapecore Qt6::Test
)
#FIXME: Too many dependencies in DataSet, there should be no need to link to flake and the entire chart shape here
chartshape_add_unit_test(TestDataSet
    TestDataSet.cpp
    LINK_LIBRARIES chartshapecore Qt6::Test
)
chartshape_add_unit_test(TestKChartModel
    TestKChartModel.cpp ModelObserver.cpp
    LINK_LIBRARIES chartshapecore Qt6::Test
)
chartshape_add_unit_test(TestTableSource
    TestTableSource.cpp
    LINK_LIBRARIES chartshapecore Qt6::Test
)
chartshape_add_unit_test(TestCellRegion
    TestCellRegion.cpp
    LINK_LIBRARIES chartshapecore Qt6::Test
)

# call: chartshape_loading_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(CHARTSHAPE_LOADING_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "shapes-chart-loading-"
    )
endmacro()

add_subdirectory( odf )

