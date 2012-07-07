
include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/libs/main ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )


########### next target ###############

set(kexihandler_query_PART_SRCS 
   kexiquerypart.cpp 
   kexiquerydesignersql.cpp 
   kexiquerydesignersqlhistory.cpp 
   kexiquerydesignerguieditor.cpp 
   kexiqueryview.cpp )

kde4_add_plugin(kexihandler_query ${kexihandler_query_PART_SRCS})



target_link_libraries(
    kexihandler_query
    kexicore
    kexidb
    ${CALLIGRADB_LIBS}
    kexiextendedwidgets
    kexidataviewcommon
    kexidatatable
    kexirelationsview
    ${KOPROPERTY_LIBS}
    kexiutils
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kexihandler_query  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexiqueryhandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
