set(kritaimageshare_PART_SRCS
    imageshare.cpp
    o2.cpp
    o2deviantart.cpp
    o2reply.cpp
    o2replyserver.cpp
    o2requestor.cpp
    simplecrypt.cpp
    dlg_login.cpp
    stash.cpp
    submitdlg.cpp
    )

kde4_add_ui_files(kritaimageshare_PART_SRCS
    dlg_login.ui
    submit.ui
)

kde4_add_plugin(kritaimageshare ${kritaimageshare_PART_SRCS})

target_link_libraries(kritaimageshare kritaui ${QT_QTSCRIPT_LIBRARY} ${QT_QTWEBKIT_LIBRARY})

install(TARGETS kritaimageshare  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  imageshare.rc  DESTINATION ${DATA_INSTALL_DIR}/kritaplugins)
install( FILES  kritaimageshare.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
