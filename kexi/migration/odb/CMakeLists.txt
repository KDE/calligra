include_directories(  ${GLIB2_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/kexi/migration/odb ${JAVA_INCLUDE} ${JNI_INCLUDE_DIRS} )

add_definitions(-DHSQLDB_LIBRARY="${HSQLDB_LIBRARY}")

add_subdirectory(reader)

set(keximigrate_odbmigrate_PART_SRCS odbmigrate.cpp)

kde4_add_plugin(keximigrate_odb ${keximigrate_odbmigrate_PART_SRCS})

target_link_libraries(keximigrate_odb
${GLIB2_LIBRARIES}
${ICONV_LIBRARIES}
keximigrate
kexidb
kexiutils
${CALLIGRADB_LIBS}
${JAVA_LIBRARIES}
${JNI_LIBRARIES}
/usr/lib64/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/xawt/libmawt.so #hardcoded!
)

install(TARGETS keximigrate_odb  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES  keximigrate_odb.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra) 
