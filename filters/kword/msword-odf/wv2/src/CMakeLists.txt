INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR/filters/kword/msword-odf/wv2} )

ADD_DEFINITIONS( -DHAVE_CONFIG_H )

SET( wv2_SOURCES 
	olestorage.cpp 
	olestream.cpp 
	word97_generated.cpp 
	word97_helper.cpp 
	word95_helper.cpp 
	word_helper.cpp 
	word95_generated.cpp 
	convert.cpp 
	global.cpp 
	parser.cpp 
	parser9x.cpp 
	parser95.cpp 
	parser97.cpp 
	parserfactory.cpp 
	ustring.cpp 
	textconverter.cpp 
	styles.cpp 
	handlers.cpp 
	properties97.cpp 
	fonts.cpp 
	lists.cpp 
	paragraphproperties.cpp 
	headers.cpp 
	headers95.cpp 
	headers97.cpp 
	footnotes97.cpp 
	functor.cpp 
	functordata.cpp 
	associatedstrings.cpp 
	fields.cpp 
	utilities.cpp 
	crc32.c 
	wvlog.cpp 
	graphics.cpp 
	wv2version.cpp 
	zcodec.cxx
    )

SET( wv2_HEADERS 
	olestorage.h 
	olestream.h 
	word95_generated.h 
	word97_generated.h 
	word_helper.h 
	parser.h 
	parserfactory.h 
	ustring.h 
	textconverter.h 
	styles.h 
	sharedptr.h 
	convert.h 
	global.h 
	handlers.h 
	fonts.h 
	lists.h 
	utilities.h 
	paragraphproperties.h 
	functor.h 
	functordata.h 
	associatedstrings.h 
	fields.h 
	wvlog.h 
	wv2version.h
	zcodec.hxx
    )

kde4_add_library( wv2 SHARED ${wv2_SOURCES} ${wv2_HEADERS} )
TARGET_LINK_LIBRARIES( wv2 ${ZLIB_LIBRARIES} ${LIBGSF_LIBRARIES} ${ICONV_LIBRARIES} ${GLIB2_LIBRARIES} ${GOBJECT_LIBRARIES} )
SET_TARGET_PROPERTIES( wv2 PROPERTIES 
			      SOVERSION ${LT_VERSION_CURRENT} 
			      VERSION ${LT_VERSION_CURRENT}.${LT_VERSION_AGE}.${LT_VERSION_REVISION} 
			      LT_VERSION_CURRENT ${LT_VERSION_CURRENT}
			      LT_VERSION_AGE ${LT_VERSION_AGE}
			      LT_VERSION_REVISION ${LT_VERSION_REVISION}
			      LT_DEPENDENCY_LIBS ${WV2_LDFLAGS}
		    )

INSTALL( TARGETS wv2
    EXPORT wvWare
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}/static
)

INSTALL( EXPORT wvWare DESTINATION lib${LIB_SUFFIX}/wvWare FILE wv2.cmake )

INSTALL( FILES ${wv2_HEADERS}
  DESTINATION include/wv2
)

# libtool files are useless for Visual C++ and Borland C++
IF( NOT MSVC AND NOT BORLAND )
  INCLUDE( ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/MacroCreateLibtoolFile.cmake )
  CREATE_LIBTOOL_FILE( wv2 /lib${LIB_SUFFIX} )
ENDIF( NOT MSVC AND NOT BORLAND )
