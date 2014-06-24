cmake_minimum_required(VERSION 2.8)

find_package(Java REQUIRED)
include(UseJava)

message (STATUS "java version= ${Java_VERSION_MINOR}")
message (STATUS "plugins dir= ${PLUGIN_INSTALL_DIR}")
message (STATUS "data dir= ${DATA_INSTALL_DIR}")

set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.7" "-target" "1.7")

add_jar(OdbReader INCLUDE_JARS hsqldb.jar SOURCES OdbReader.java)

install( FILES  OdbReader.jar  DESTINATION ${PLUGIN_INSTALL_DIR}) 
install( FILES  hsqldb.jar  DESTINATION ${PLUGIN_INSTALL_DIR}) 

