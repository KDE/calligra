include_directories(${QT_INCLUDES} ${KDE4_INCLUDES})

set(kocmpx_SRCS
    KoCmpx.cpp
)

kde4_add_library(kocmpx SHARED ${kocmpx_SRCS})

target_link_libraries(kocmpx ${CMPX_LIBRARIES} ${KDE4_KDECORE_LIBS}
                             ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})

install(TARGETS kocmpx ${INSTALL_TARGETS_DEFAULT_ARGS})

install(
    FILES
    cmpx_export.h
    KoCmpx.h
    DESTINATION
    ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)