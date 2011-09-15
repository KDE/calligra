install(FILES scripts.rc DESTINATION ${DATA_INSTALL_DIR}/words/scripts)

#install(FILES variable_readfile.py DESTINATION ${DATA_INSTALL_DIR}/words/scripts/variables)

install(FILES
    importfile.py
    importdoxyxml.py importdoxyxmloptions.ui importdoxyxmlstyle.ui
    ooimport.py ooimportconfig.ui ooimport.ui oouno.py
    exportfile.py
    kwreportlab.py
    xmlviewer.py
    doctree.rb
    onlinehelp.py
    DESTINATION ${DATA_INSTALL_DIR}/words/scripts/extensions)

install(FILES
    sample_cursor.rb
    sample_actions.py
    sample_toolactions.py
    sample_progressbar.py
    sample_text.py
    sample_lists_html.py
    sample_lists_cursor.py
    sample_tables.py
    sample_variables.py
    sample_insertshape.py
    sample_allshapes.py
    DESTINATION ${DATA_INSTALL_DIR}/words/scripts/samples)
