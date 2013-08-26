

set(kritawaveletfilter_PART_SRCS
    waveletdecompose.cpp
    kis_wdg_wavelet_decompose.cpp
    kis_wavelet_decompose_filter.cpp
    )


kde4_add_ui_files(kritawaveletfilter_PART_SRCS
    wdg_wavelet_decompose.ui
    )

kde4_add_plugin(kritawaveletfilter ${kritawaveletfilter_PART_SRCS})



target_link_libraries(kritawaveletfilter kritaui)

install(TARGETS kritawaveletfilter  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES DESTINATION ${SERVICES_INSTALL_DIR})
