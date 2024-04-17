set(TestLoading_SRCS TestLoading.cpp ../TestLoadingBase.cpp ../TestMultipleFiles.cpp)
chartshape_loading_add_unit_test(bubble-chart
    ${TestLoading_SRCS}
    LINK_LIBRARIES chartshapecore flake koodf komain Qt6::Test
)
