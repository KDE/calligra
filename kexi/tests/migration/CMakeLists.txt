

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core ${KDE4_INCLUDES}  )

# temp
#ADD_DEFINITIONS( -DNO_GUI=1 )

########### next target ###############

set(xbasemigrationtest_SRCS main.cpp )

kde4_add_executable(xbasemigrationtest NOGUI ${xbasemigrationtest_SRCS})

target_link_libraries(xbasemigrationtest ${KDE4_KDECORE_LIBS} kexicore kexidb ${CALLIGRADB_LIBS} keximigrate)

########### next target ###############

set(readertest_SRCS readertest.cpp )

kde4_add_executable(readertest ${readertest_SRCS})

target_link_libraries(readertest  ${KDE4_KDECORE_LIBS} keximigrate kexicore kexidb ${CALLIGRADB_LIBS} keximain)

########### install files ###############
