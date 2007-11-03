install(FILES
    scripts.rc
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts)

install(FILES
    csvimport.py csvexport.py csvoptions.ui
    vcardimport.py
    kexidbimport.py kexidbexport.py
    htmlexport.py htmlexportinfos.ui
    odfpyexport.py
    logger.py
    xmlviewer.py
    myorca.py
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts/extensions)

install(FILES
    functions.py
    yfinance.py yweather.py
    pytime.py pyregexp.py
    DESTINATION ${DATA_INSTALL_DIR}/kspread/scripts/functions)
