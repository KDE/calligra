include_directories(
    ${QT_INCLUDES}
    ${KDGANTT_INCLUDES}
)

add_definitions(-DKDAB_NO_UNIT_TESTS -DKDGANTT_SHAREDLIB -DKDGANTT_BUILD_KDGANTT_LIB)

set( kdgantt_LIB_SRCS
    kdganttglobal.cpp
    kdganttview.cpp
    kdganttstyleoptionganttitem.cpp
    kdganttgraphicsview.cpp
    kdganttabstractrowcontroller.cpp
    kdgantttreeviewrowcontroller.cpp
    kdganttlistviewrowcontroller.cpp
    kdganttgraphicsscene.cpp
    kdganttgraphicsitem.cpp
    kdganttconstraint.cpp
    kdganttconstraintproxy.cpp
    kdganttconstraintgraphicsitem.cpp
    kdganttitemdelegate.cpp
    kdganttforwardingproxymodel.cpp
    kdganttsummaryhandlingproxymodel.cpp
    kdganttproxymodel.cpp
    kdganttconstraintmodel.cpp
    kdganttabstractgrid.cpp
    kdganttdatetimegrid.cpp
    kdganttlegend.cpp
    kdgantttimescalezoomdialog.cpp
)

# The reason why we call it calligrakdgantt instead of just kdgantt is
# the following comment from Dag Andersen:
#   "there are other variants of kdgantt out there (like in korganizer).
#    So to avoid any potential clash I think the lib should be renamed
#    e.g. calligrakdgantt."

kde4_add_library(calligrakdgantt SHARED ${kdgantt_LIB_SRCS})

target_link_libraries(calligrakdgantt ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})
target_link_libraries(calligrakdgantt LINK_INTERFACE_LIBRARIES ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

set_target_properties(calligrakdgantt PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION}
    SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS calligrakdgantt ${INSTALL_TARGETS_DEFAULT_ARGS} )
