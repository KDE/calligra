
include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES} )

# temp
#ADD_DEFINITIONS( -DNO_GUI=1 )

########### next target ###############

set(kexidbtest_SRCS main.cpp )

kde4_add_executable(kexidbtest NOGUI ${kexidbtest_SRCS})

target_link_libraries(kexidbtest ${KDE4_KDEUI_LIBS} kexidb kexiextendedwidgets kexidatatable kexicore )
