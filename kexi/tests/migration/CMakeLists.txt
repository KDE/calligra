

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core   )

# temp
#ADD_DEFINITIONS( -DNO_GUI=1 )

########### next target ###############

set(xbasemigrationtest_SRCS main.cpp )

add_executable(xbasemigrationtest NOGUI ${xbasemigrationtest_SRCS})

target_link_libraries(xbasemigrationtest kexicore Predicate keximigrate)

########### next target ###############

set(readertest_SRCS readertest.cpp )

add_executable(readertest ${readertest_SRCS})

target_link_libraries(readertest   keximigrate kexicore Predicate keximain)

########### install files ###############
