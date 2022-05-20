install(FILES scripts.rc DESTINATION ${KDE_INSTALL_DATADIR}/calligrawords/scripts)

#install(FILES variable_readfile.py DESTINATION ${KDE_INSTALL_DATADIR}/calligrawords/scripts/variables)

install(FILES
    importfile.py
    importdoxyxml.py importdoxyxmloptions.ui importdoxyxmlstyle.ui
    ooimport.py ooimportconfig.ui ooimport.ui oouno.py
    exportfile.py
    kwreportlab.py
    xmlviewer.py
    doctree.rb
    onlinehelp.py
    DESTINATION ${KDE_INSTALL_DATADIR}/calligrawords/scripts/extensions)

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
    DESTINATION ${KDE_INSTALL_DATADIR}/calligrawords/scripts/samples)
