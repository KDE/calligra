# Generate parsers for some Word structures.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)

#cmake_policy CMP0017 was introduced in version 2.8.4
if(${CMAKE_VERSION} VERSION_GREATER 2.8.3)
    cmake_policy(SET CMP0017 NEW)
endif()

SET( scanner_word95
	${CMAKE_CURRENT_BINARY_DIR}/word95_generated.h
	${CMAKE_CURRENT_BINARY_DIR}/word95_generated.cpp
	${CMAKE_CURRENT_BINARY_DIR}/word95_test.cpp
	)

SET( scanner_word97
	${CMAKE_CURRENT_BINARY_DIR}/word97_generated.h
	${CMAKE_CURRENT_BINARY_DIR}/word97_generated.cpp
	${CMAKE_CURRENT_BINARY_DIR}/word97_test.cpp
	)

SET( converter
	${CMAKE_CURRENT_BINARY_DIR}/convert.h
	${CMAKE_CURRENT_BINARY_DIR}/convert.cpp
	)

ADD_CUSTOM_COMMAND(
	OUTPUT ${scanner_word95}
	COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && perl generate.pl generator_wword6.htm Word95 && mv word95_*.* ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/generator_wword6.htm
                ${CMAKE_CURRENT_SOURCE_DIR}/template-Word95.h
                ${CMAKE_CURRENT_SOURCE_DIR}/template-Word95.cpp
	)

ADD_CUSTOM_COMMAND(
	OUTPUT ${scanner_word97}
	COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && perl generate.pl generator_wword8.htm Word97 && mv word97_*.* ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/generator_wword8.htm
                ${CMAKE_CURRENT_SOURCE_DIR}/template-Word97.h
                ${CMAKE_CURRENT_SOURCE_DIR}/template-Word97.cpp
	)

ADD_CUSTOM_COMMAND(
	OUTPUT ${converter}
	COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && perl converter.pl generator_wword6.htm generator_wword8.htm && mv convert.* ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/converter.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/generator_wword6.htm
                ${CMAKE_CURRENT_SOURCE_DIR}/generator_wword8.htm
                ${CMAKE_CURRENT_SOURCE_DIR}/template-conv.h
                ${CMAKE_CURRENT_SOURCE_DIR}/template-conv.cpp
	)

ADD_CUSTOM_TARGET( generator ALL DEPENDS ${scanner_word95} ${scanner_word97} ${converter} )

