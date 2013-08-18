set(libki18n_SRCS
    klocalizedstring.cpp
    kcatalog.cpp
    kuitmarkup.cpp
    common_helpers.cpp
)

add_library(KI18n ${libki18n_SRCS})
generate_export_header(KI18n)

target_link_libraries(KI18n LINK_PUBLIC  Qt5::Core
                            LINK_PRIVATE Qt5::Xml)

set_target_properties(KI18n PROPERTIES VERSION   ${KI18N_VERSION_STRING}
                                       SOVERSION ${KI18N_SOVERSION}
)

install(TARGETS KI18n EXPORT KI18nTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    klocalizedstring.h
    kuitmarkup.h
    ${CMAKE_CURRENT_BINARY_DIR}/ki18n_export.h
    DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)


### ktranscript plugin

# kjs hash stuff for transcript plugin
#set(CREATE_HASH_TABLE ${KJS_SOURCE_DIR}/src/kjs/create_hash_table)

#add_custom_command(
#    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ktranscript.lut.h
#    COMMAND ${PERL_EXECUTABLE} ${CREATE_HASH_TABLE}
#            ${CMAKE_CURRENT_SOURCE_DIR}/ktranscript.cpp
#            > ${CMAKE_CURRENT_BINARY_DIR}/ktranscript.lut.h
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ktranscript.cpp
#)
#macro_add_file_dependencies(${CMAKE_CURRENT_SOURCE_DIR}/ktranscript.cpp ${CMAKE_CURRENT_BINARY_DIR}/ktranscript.lut.h)

#set(ktranscript_LIB_SRCS
#    ktranscript.cpp
#    common_helpers.cpp
#)

#add_library(ktranscript MODULE ${ktranscript_LIB_SRCS})
#set_target_properties(ktranscript PROPERTIES PREFIX "") # remove lib prefix
#target_link_libraries(ktranscript KJS Qt5::Core)

#install(TARGETS ktranscript DESTINATION ${PLUGIN_INSTALL_DIR})

