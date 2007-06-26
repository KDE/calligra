install(FILES
    scripts.rc
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts)

install(FILES
    kexidbimport.py
    csvimport.py csvexport.py csvoptions.ui
    htmlexport.py htmlexportinfos.ui
    odfpyexport.py
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts/extensions)

install(FILES
    functions.py
    yfinance.py yweather.py
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts/functions)
