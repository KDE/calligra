
macro_optional_find_package(JNI)
macro_log_feature(JNI_FOUND "Java JNI" "Java native interface" "http://www.oracle.com/" FALSE "" "Required by Plan mpxj import filter")

if(JNI_FOUND)
    macro_optional_add_subdirectory( planconvert )
    macro_optional_add_subdirectory( import )
endif(JNI_FOUND)
