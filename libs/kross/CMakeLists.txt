include_directories(${KOMAIN_INCLUDES})

set(kokross_LIB_SRCS
    KoScriptingModule.cpp
    KoScriptingDocker.cpp
    KoScriptManager.cpp
    KoScriptManagerAdd.cpp
    KoScriptingPart.cpp
    KoScriptingOdf.cpp
    KoKrossDebug.cpp
)
add_library(kokross SHARED ${kokross_LIB_SRCS})
generate_export_header(kokross)

target_link_libraries(kokross
PUBLIC
    komain
    KF5::KrossUi
PRIVATE
    KF5::KIOFileWidgets
)

set_target_properties(kokross PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS kokross ${INSTALL_TARGETS_DEFAULT_ARGS})
