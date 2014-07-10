macro_optional_find_package(PythonLibrary)
macro_log_feature(PYTHON_LIBRARY "PythonLibrary" "Python Library" FALSE "" "Required by the Krita PyQt plugin")

macro_optional_find_package(SIP 4.7.1)
macro_log_feature(SIP_FOUND "SIP" "Support for SIP Python bindings" FALSE "" "Required by the Krita PyQt plugin")

macro_optional_find_package(PyQt4 4.3.1)
macro_log_feature(PYQT4_FOUND "PyQt4" "Python bindings for Qt4" FALSE "" "Required by the Krita PyQt plugin")

if (NOT EXISTS ${PYQT4_SIP_DIR}/QtCore/QtCoremod.sip)
    message(WARNING "krita-pyqt plugin needs the PyQt4 development sip file QtCoremod.sip")
    return()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${SIP_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH})

#add_subdirectory(sip)
add_subdirectory(src)
