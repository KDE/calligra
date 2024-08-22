# To hide all the warnings from the generated itex2MML code
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-unused-parameter)
endif ()


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)
#if(CMAKE_COMPILER_IS_GNUCXX)
#    list(APPEND CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage")
#    list(APPEND CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
#endif()

set(RtfReader_LIB_SOURCES
        rtfdebug.cpp
	rtfreader.cpp
	controlword.cpp
	AbstractRtfOutput.cpp
	AuthorPcdataDestination.cpp
	CategoryPcdataDestination.cpp
	ColorTableDestination.cpp
	CommentPcdataDestination.cpp
	CompanyPcdataDestination.cpp
	Destination.cpp
	DocumentCommentPcdataDestination.cpp
	DocumentDestination.cpp
	FontTableDestination.cpp
	GeneratorPcdataDestination.cpp
	HLinkBasePcdataDestination.cpp
	IgnoredDestination.cpp
	InfoDestination.cpp
	InfoTimeDestination.cpp
	InfoCreatedTimeDestination.cpp
	InfoPrintedTimeDestination.cpp
	InfoRevisedTimeDestination.cpp
	KeywordsPcdataDestination.cpp
	ManagerPcdataDestination.cpp
	OperatorPcdataDestination.cpp
	PcdataDestination.cpp
	PictDestination.cpp
	SubjectPcdataDestination.cpp
	StyleSheetDestination.cpp
	TextDocumentRtfOutput.cpp
	TitlePcdataDestination.cpp
	Token.cpp
	Tokenizer.cpp
	UserPropsDestination.cpp
)

add_library(RtfReader SHARED ${RtfReader_LIB_SOURCES})
generate_export_header( RtfReader )

target_link_libraries(RtfReader Qt6::Gui)

set_target_properties(RtfReader PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION} )
install(TARGETS RtfReader ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
