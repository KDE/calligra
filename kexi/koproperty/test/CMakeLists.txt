if(KDE4_BUILD_TESTS)

include_directories( ${CMAKE_SOURCE_DIR}/kexi/koproperty ${KOMAIN_INCLUDES} )

########### next target ###############

set(propertytest_SRCS main.cpp test.cpp )

kde4_add_executable(propertytest2 NOGUI ${propertytest_SRCS})

target_link_libraries(propertytest2  ${KDE4_KDEUI_LIBS} koproperty )
endif(KDE4_BUILD_TESTS)

########### install files ###############


