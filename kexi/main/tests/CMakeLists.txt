calligra_build_test(GlobalSearch)
target_link_libraries(TestGlobalSearch Qt5::Test  ${CALLIGRADB_LIBS}
                                        keximain kexicore kexiextendedwidgets)

