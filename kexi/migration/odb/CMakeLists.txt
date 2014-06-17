# if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
#     add_definitions(-Wno-unused-but-set-variable)
#     add_definitions(-Wno-missing-format-attribute)
#     add_definitions(-Wno-sign-compare)
#     add_definitions(-Wno-unused-parameter)
# endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)


include_directories(  ${GLIB2_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/kexi/migration/odb ${JAVA_INCLUDE} ${JNI_INCLUDE_DIRS} )

include(UseJava)


    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")


set(keximigrate_odbmigrate_PART_SRCS odbmigrate.cpp
)

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
)

install(TARGETS keximigrate_odb  DESTINATION ${PLUGIN_INSTALL_DIR})



install( FILES  keximigrate_odb.desktop  DESTINATION ${SERVICES_INSTALL_DIR}) 
