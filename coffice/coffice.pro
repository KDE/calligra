TEMPLATE = subdirs

CONFIG += ordered

QMAKE_CACHE = "$${OUT_PWD}/.qmake.cache"
TOP_SOURCE_DIR = $${IN_PWD}
TOP_BUILD_DIR = $${OUT_PWD}
system('echo \"\" > $${QMAKE_CACHE}')
system('echo TOP_SOURCE_DIR=$${TOP_SOURCE_DIR} >> $${QMAKE_CACHE}')
system('echo TOP_BUILD_DIR=$${TOP_BUILD_DIR} >> $${QMAKE_CACHE}')

include(common.pri)

SUBDIRS += calligralibs
SUBDIRS += calligraplugins
SUBDIRS += calligrawords
SUBDIRS += coffice
