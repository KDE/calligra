project( kasten_core_tests )


set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

set( SUTDIR ${CMAKE_CURRENT_SOURCE_DIR}/.. )

include_directories(
  ${SUTDIR}/..
  ${SUTDIR}
)

########### util ###############

set( kastencoretestutil_LIB_SRCS
  util/filesystem.cpp
)

kde4_add_library( kastencoretestutil  STATIC ${kastencoretestutil_LIB_SRCS} )

########### io ###############

set( kastencoretestio_LIB_SRCS
  testdocumentfileloadthread.cpp
  testdocumentfilereloadthread.cpp
  testdocumentfilewritethread.cpp
  testdocumentfileloadjob.cpp
  testdocumentfileconnectjob.cpp
  testdocumentfilewritejob.cpp
  testdocumentfilereloadjob.cpp
  testdocumentfilewritetojob.cpp
)

kde4_add_library( kastencoretestio  STATIC ${kastencoretestio_LIB_SRCS} )

########### testdocumenttest ###############

set( testdocumenttest_SRCS
  testdocument.cpp
  testdocumenttest.cpp
)

kde4_add_unit_test( testdocumenttest
  TESTNAME libkasten-core-testdocumenttest
  ${testdocumenttest_SRCS}
)
target_link_libraries( testdocumenttest
  ${kastencore_LIB}
  ${KDE4_KIO_LIBS}
  ${QT_QTTEST_LIBRARY}
)

########### testdocumentfilesynchronizertest ###############

set( testdocumentfilesynchronizertest_SRCS
  testdocument.cpp
  testdocumentfilesynchronizer.cpp
  testdocumentfilesynchronizertest.cpp
)

kde4_add_unit_test( testdocumentfilesynchronizertest
  TESTNAME libkasten-core-testdocumentfilesynchronizertest
  ${testdocumentfilesynchronizertest_SRCS}
)
target_link_libraries( testdocumentfilesynchronizertest
  kastencoretestio
  ${kastencore_LIB}
  kastencoretestutil
  ${KDE4_KIO_LIBS}
  ${QT_QTTEST_LIBRARY}
)

############ testdocumentfilesynchronizerfactorytest ###############

set( testdocumentfilesynchronizerfactorytest_SRCS
  testdocument.cpp
  testdocumentfilesynchronizer.cpp
  testdocumentfilesynchronizerfactory.cpp
  testdocumentfilesynchronizerfactorytest.cpp
)

kde4_add_unit_test( testdocumentfilesynchronizerfactorytest
  TESTNAME libkasten-core-testdocumentfilesynchronizerfactorytest
  ${testdocumentfilesynchronizerfactorytest_SRCS}
)
target_link_libraries( testdocumentfilesynchronizerfactorytest
  kastencoretestio
  ${kastencore_LIB}
  kastencoretestutil
  ${KDE4_KIO_LIBS}
  ${QT_QTTEST_LIBRARY}
)

########### documentmanagertest ###############

set( documentmanagertest_SRCS
  testdocument.cpp
  documentmanagertest.cpp
)

kde4_add_unit_test( documentmanagertest
  TESTNAME libkasten-core-documentmanagertest
  ${documentmanagertest_SRCS}
)
target_link_libraries( documentmanagertest
  ${kastencore_LIB}
  ${KDE4_KIO_LIBS}
  ${QT_QTTEST_LIBRARY}
)
