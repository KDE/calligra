# temp
#ADD_DEFINITIONS( -DNO_GUI=1 )

########### next target ###############

set(kexidbtest_SRCS main.cpp )

add_executable(kexidbtest NOGUI ${kexidbtest_SRCS})

target_link_libraries(kexidbtest KDb kexiextendedwidgets kexidatatable kexicore kexidataviewcommon)
