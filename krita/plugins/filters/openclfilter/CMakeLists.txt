set(kritaopenclfilter_PART_SRCS
    kritaopencl.cpp
    kritaopenclfilter.cpp
    kritawidgetopencl.cpp
    )

kde4_add_plugin(kritaopenclfilter ${kritaopenclfilter_PART_SRCS})


target_link_libraries(kritaopenclfilter kritaui OpenCL QtOpenCL)

install(TARGETS kritaopenclfilter  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kritaopenclfilter.desktop  DESTINATION ${SERVICES_INSTALL_DIR})