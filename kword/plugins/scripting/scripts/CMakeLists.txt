install(FILES scripts.rc DESTINATION ${DATA_INSTALL_DIR}/kword/scripts)

install(FILES
    importfile.py
    ooimport.py ooimportconfig.ui ooimport.ui oouno.py
    exportfile.py
    kwreportlab.py
    xmlviewer.py
    doctree.rb
    DESTINATION ${DATA_INSTALL_DIR}/kword/scripts/extensions)

install(FILES
    sample_actions.py
    sample_progressbar.py
    DESTINATION ${DATA_INSTALL_DIR}/kword/scripts/samples)
