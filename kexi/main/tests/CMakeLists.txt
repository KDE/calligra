calligra_build_test(GlobalSearch)
target_link_libraries(TestGlobalSearch ${QT_QTTEST_LIBRARY} ${KDE4_KDECORE_LIBS} ${CALLIGRADB_LIBS}
                                       ${KDE4_KDEUI_LIBS} keximain kexicore kexiextendedwidgets)

