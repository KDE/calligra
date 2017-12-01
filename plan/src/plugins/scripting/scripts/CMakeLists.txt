install(FILES scripts.rc DESTINATION ${DATA_INSTALL_DIR}/calligraplan/scripts)

install(FILES
    resourcesimport.py
    busyinfoexport.py
    busyinfoimport.py
    busyinfoclear.py
    plan2csvexport.py
    plan2odsexport.py
    DESTINATION ${DATA_INSTALL_DIR}/calligraplan/scripts/extensions)

install(FILES
    sample_project.py
    sample_busyinfocheck.py
    DESTINATION ${DATA_INSTALL_DIR}/calligraplan/scripts/samples)
