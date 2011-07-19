# CMakeLists.txt
# This file runs a python script for conversion of files to ODF formats and validation of the resulting ODF files against RelaxNG
# NOTE! jing.jar has to reside in the same folder as the python script for this to work
# Stores the list of all files from PATH_TO_TEST_FILES in the variable list_of_test_files

set(PATH_TO_TEST_FILES "" CACHE FILEPATH "This variable is used to set the path to the calligra conversion and validation test files - used under calligra/tools/scripts/")
message( status ${PATH_TO_TEST_FILES} )
IF ( EXISTS ${PATH_TO_TEST_FILES} )
	file(GLOB_RECURSE list_of_test_files RELATIVE ${PATH_TO_TEST_FILES} "${PATH_TO_TEST_FILES}/*")

	foreach(test_file ${list_of_test_files})
		
		IF ( NOT ( ${test_file} MATCHES ".*[.]svn.*" ) AND ( ${test_file} MATCHES ".*[.](odt|odp|ods|xls|xlsx|ppt|pptx|doc|dox|txt)" ))
			ADD_TEST(${test_file} python ${CMAKE_CURRENT_SOURCE_DIR}/convertAndValidateODF.py no ${PATH_TO_TEST_FILES} ${test_file}) 
		ENDIF()
	endforeach()

        # Use for cstester
        option(ENABLE_CSTESTER_TESTING "Enable testing with cstester" OFF)

        IF(ENABLE_CSTESTER_TESTING)
                foreach(test_file ${list_of_test_files})

                        IF ( NOT ( ${test_file} MATCHES ".*[.]svn.*" ) AND ( ${test_file} MATCHES ".*[.](odt|odp|ods|xls|xlsx|ppt|pptx|doc|dox|txt)" ))
                                IF ( NOT EXISTS ${PATH_TO_CSTESTER_RESULTS_FILES}/${test_file}.check )
                                        execute_process(COMMAND cstester --create ${PATH_TO_TEST_FILES}/${test_file})
                                ENDIF()

                                ADD_TEST(${test_file}_verify cstester --verify ${PATH_TO_TEST_FILES}/${test_file})
                                set_property(TEST ${test_file}_create PROPERTY LABELS cstester)
                        ENDIF()
                endforeach()
        ENDIF()
        
ENDIF()
#EOF
