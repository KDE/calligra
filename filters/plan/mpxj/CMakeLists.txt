if(SHOULD_BUILD_FILTER_MPXJ_IMPORT)

# macro_optional_find_package(JNI)
# macro_log_feature(JNI_FOUND "Java JNI" "Java native interface" "http://www.oracle.com/" FALSE "" "Required by Plan mpxj import filter")

# if(JNI_FOUND)
# Note: no need to check for JNI currently, JNI not used,
# cmp. "NOTE:" in mpxjimport.cpp
    add_subdirectory( planconvert )
    add_subdirectory( import )
# endif(JNI_FOUND)

endif(SHOULD_BUILD_FILTER_MPXJ_IMPORT)
