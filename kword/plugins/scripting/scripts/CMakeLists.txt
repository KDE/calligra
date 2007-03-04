install(FILES scripts.rc DESTINATION ${DATA_INSTALL_DIR}/kword/scripts)
install(FILES scripts.rc DESTINATION ${DATA_INSTALL_DIR}/kword/scripts)
install(FILES
    variable_readfile.py
    DESTINATION ${DATA_INSTALL_DIR}/kword/scripts/variables)

install(FILES
    sample_importfile.py
    sample_exportfile.py
    sample_doctree.rb
    sample_cursor.rb
    sample_actions.py
    sample_progressbar.py
    sample_text.py
    sample_lists_html.py
    sample_lists_cursor.py
    sample_tables.py
    sample_variables.py
    sample_insertshape.py
    sample_allshapes.py
    DESTINATION ${DATA_INSTALL_DIR}/kword/scripts/extensions)
