install(FILES
    scripts.rc
    DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/scripts)

install(FILES
    csvimport.py csvexport.py csvoptions.ui
    vcardimport.py
    htmlexport.py htmlexportinfos.ui
    odfpyexport.py
    logger.py
    xmlviewer.py
    myorca.py
    DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/scripts/extensions)

install(FILES
    functions.py
    rpyfunctions.py
    yfinance.py yweather.py
    pytime.py pyregexp.py
    DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/scripts/functions)

install(FILES
    consoledocker.py
    snippetsdocker.rb
    DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/scripts/docker)
