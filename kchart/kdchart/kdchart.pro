include( variables.pri )

CONFIG += ordered

TEMPLATE=subdirs
SUBDIRS = src examples uitools plugins

contains($$list($$[QT_VERSION]), 4.2.*) { SUBDIRS += kdchartserializer }
contains($$list($$[QT_VERSION]), 4.3.*) { SUBDIRS += kdchartserializer }

