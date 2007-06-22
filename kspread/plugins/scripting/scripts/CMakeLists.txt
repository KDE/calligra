install(FILES
    scripts.rc
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts)

install(FILES
    csvimport.py csvexport.py csvoptions.ui
    htmlexport.py htmlexportinfos.ui
    odfpyexport.py
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts/extensions)

install(FILES
    functions.py
    yweather.py
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts/functions)
