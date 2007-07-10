

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/core ${KDE4_INCLUDES}  )

# temp
ADD_DEFINITIONS( -DNO_GUI=1 )

set(kexialtertabletest_SRCS main.cpp )


kde4_add_executable(kexialtertabletest ${kexialtertabletest_SRCS})

target_link_libraries(kexialtertabletest  ${KDE4_KDECORE_LIBS} kexidb ) 

