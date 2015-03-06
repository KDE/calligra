

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core ${KDE4_INCLUDES}  )

# temp
#ADD_DEFINITIONS( -DNO_GUI=1 )

########### next target ###############

set(xbasemigrationtest_SRCS main.cpp )

add_executable(xbasemigrationtest NOGUI ${xbasemigrationtest_SRCS})

target_link_libraries(xbasemigrationtest  kexicore kexidb ${CALLIGRADB_LIBS} keximigrate)

########### next target ###############

set(readertest_SRCS readertest.cpp )

add_executable(readertest ${readertest_SRCS})

target_link_libraries(readertest   keximigrate kexicore kexidb ${CALLIGRADB_LIBS} keximain)

########### install files ###############
