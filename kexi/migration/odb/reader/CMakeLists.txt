set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.7" "-target" "1.7")

add_jar(OdbReader INCLUDE_JARS ${HSQLDB_LIBRARY} SOURCES OdbReader.java)

install( FILES  ${CMAKE_CURRENT_BINARY_DIR}/OdbReader.jar  DESTINATION ${LIB_INSTALL_DIR}) 
