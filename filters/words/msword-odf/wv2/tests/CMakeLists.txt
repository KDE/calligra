IF( EXISTS ${wvWare_BINARY_DIR}/config.h )
  ADD_DEFINITIONS( -DHAVE_CONFIG_H )
ENDIF( EXISTS ${wvWare_BINARY_DIR}/config.h )

INCLUDE_DIRECTORIES( ${wvWare_BINARY_DIR} ${wvWare_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR} )

SET( oletest_SOURCES oletest.cpp )
ADD_EXECUTABLE( oletest EXCLUDE_FROM_ALL ${oletest_SOURCES} )
TARGET_LINK_LIBRARIES( oletest wv2 ${LIBGSF_LIBRARIES} )

SET( word97_test_SOURCES word97_test.cpp )
ADD_EXECUTABLE( word97_test EXCLUDE_FROM_ALL ${word97_test_SOURCES} )
TARGET_LINK_LIBRARIES( word97_test wv2 ${LIBGSF_LIBRARIES} )

SET( word95_test_SOURCES word95_test.cpp )
ADD_EXECUTABLE( word95_test EXCLUDE_FROM_ALL ${word95_test_SOURCES} )
TARGET_LINK_LIBRARIES( word95_test wv2 ${LIBGSF_LIBRARIES} )

SET( iconvtest_SOURCES iconvtest.cpp )
ADD_EXECUTABLE( iconvtest EXCLUDE_FROM_ALL ${iconvtest_SOURCES} )
TARGET_LINK_LIBRARIES( iconvtest wv2 ${LIBGSF_LIBRARIES} )

SET( parsertest_SOURCES parsertest.cpp )
ADD_EXECUTABLE( parsertest EXCLUDE_FROM_ALL ${parsertest_SOURCES} )
TARGET_LINK_LIBRARIES( parsertest wv2 ${LIBGSF_LIBRARIES} )

SET( parsertest_mem_SOURCES parsertest_mem.cpp )
ADD_EXECUTABLE( parsertest_mem EXCLUDE_FROM_ALL ${parsertest_mem_SOURCES} )
TARGET_LINK_LIBRARIES( parsertest_mem wv2 ${LIBGSF_LIBRARIES} )

SET( helpertest_SOURCES helpertest.cpp )
ADD_EXECUTABLE( helpertest EXCLUDE_FROM_ALL ${helpertest_SOURCES} )
TARGET_LINK_LIBRARIES( helpertest wv2 ${LIBGSF_LIBRARIES} )

SET( ustringtest_SOURCES ustringtest.cpp )
ADD_EXECUTABLE( ustringtest EXCLUDE_FROM_ALL ${ustringtest_SOURCES} )
TARGET_LINK_LIBRARIES( ustringtest wv2 ${LIBGSF_LIBRARIES} )

SET( sharedptrtest_SOURCES sharedptrtest.cpp )
ADD_EXECUTABLE( sharedptrtest EXCLUDE_FROM_ALL ${sharedptrtest_SOURCES} )
TARGET_LINK_LIBRARIES( sharedptrtest wv2 ${LIBGSF_LIBRARIES} )

SET( handlertest_SOURCES handlertest.cpp )
ADD_EXECUTABLE( handlertest EXCLUDE_FROM_ALL ${handlertest_SOURCES} )
TARGET_LINK_LIBRARIES( handlertest wv2 ${LIBGSF_LIBRARIES} )

# Add a global 'make test' target
ADD_CUSTOM_TARGET( test DEPENDS oletest word97_test word95_test iconvtest parsertest parsertest_mem helpertest ustringtest sharedptrtest handlertest )
