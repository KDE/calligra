# Generate parsers for some Word structures.
cmake_minimum_required(VERSION 3.2.0)

set( scanner_word95
	${CMAKE_CURRENT_BINARY_DIR}/word95_generated.h
	${CMAKE_CURRENT_BINARY_DIR}/word95_generated.cpp
	${CMAKE_CURRENT_BINARY_DIR}/word95_test.cpp
	)

set( scanner_word97
	${CMAKE_CURRENT_BINARY_DIR}/word97_generated.h
	${CMAKE_CURRENT_BINARY_DIR}/word97_generated.cpp
	${CMAKE_CURRENT_BINARY_DIR}/word97_test.cpp
	)

set( converter
	${CMAKE_CURRENT_BINARY_DIR}/convert.h
	${CMAKE_CURRENT_BINARY_DIR}/convert.cpp
	)

add_custom_command(
	OUTPUT ${scanner_word95}
	COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && perl generate.pl generator_wword6.htm Word95 && mv word95_*.* ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/generator_wword6.htm
                ${CMAKE_CURRENT_SOURCE_DIR}/template-Word95.h
                ${CMAKE_CURRENT_SOURCE_DIR}/template-Word95.cpp
	)

add_custom_command(
	OUTPUT ${scanner_word97}
	COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && perl generate.pl generator_wword8.htm Word97 && mv word97_*.* ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/generator_wword8.htm
                ${CMAKE_CURRENT_SOURCE_DIR}/template-Word97.h
                ${CMAKE_CURRENT_SOURCE_DIR}/template-Word97.cpp
	)

add_custom_command(
	OUTPUT ${converter}
	COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && perl converter.pl generator_wword6.htm generator_wword8.htm && mv convert.* ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/converter.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/generator_wword6.htm
                ${CMAKE_CURRENT_SOURCE_DIR}/generator_wword8.htm
                ${CMAKE_CURRENT_SOURCE_DIR}/template-conv.h
                ${CMAKE_CURRENT_SOURCE_DIR}/template-conv.cpp
	)

add_custom_target( generator ALL DEPENDS ${scanner_word95} ${scanner_word97} ${converter} )

