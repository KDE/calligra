
set(kritarijn_PART_SRCS rijn.cc kis_rijn_sketch_action.cpp)

include_directories(${OPENRIJN_INCLUDE_DIRS})

# kde4_add_ui_files(kritarijn_PART_SRCS myuifile.ui )

kde4_add_plugin(kritarijn ${kritarijn_PART_SRCS})

target_link_libraries(kritarijn kritaui ${OPENRIJN_LDFLAGS} ${QTRIJN_LDFLAGS})

install(TARGETS kritarijn  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  rijn.rc  DESTINATION ${DATA_INSTALL_DIR}/kritaplugins)
install( FILES  kritarijn.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

