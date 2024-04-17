set( TestLoading_SRCS TestLoading.cpp ../TestLoadingBase.cpp ../../../ChartDocument.cpp )
chartshape_loading_add_unit_test(@@NAME@@
    ${TestLoading_SRCS}
    LINK_LIBRARIES chartshapecore flake koodf komain Qt6::Test
)
