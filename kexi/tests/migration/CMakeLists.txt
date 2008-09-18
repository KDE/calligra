

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core ${KDE4_INCLUDES}  )

# temp
#ADD_DEFINITIONS( -DNO_GUI=1 )

########### next target ###############

set(xbasemigrationtest_SRCS main.cpp )

kde4_add_executable(xbasemigrationtest NOGUI ${xbasemigrationtest_SRCS})

target_link_libraries(xbasemigrationtest ${KDE4_KDECORE_LIBS} kexidb keximigrate )


########### install files ###############
