install(FILES
    scripts.rc
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts)

install(FILES
    myKSpread.py
    csvexport.py csvexportoptions.ui
    htmlexport.py htmlexportinfos.ui
    odfpyexport.py
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts/extensions)

install(FILES
    functions.py
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts/functions)
