include_directories(${CMAKE_SOURCE_DIR}/filters/kpresenter/powerpoint/libppt/ ${CMAKE_BINARY_DIR}/filters/)

set(libppt_SRCS 
		${CMAKE_SOURCE_DIR}/filters/kpresenter/powerpoint/libppt/slide.cpp 
		${CMAKE_SOURCE_DIR}/filters/kpresenter/powerpoint/libppt/objects.cpp 
		${CMAKE_SOURCE_DIR}/filters/kpresenter/powerpoint/libppt/presentation.cpp 
		${CMAKE_SOURCE_DIR}/filters/kpresenter/powerpoint/libppt/pole.cpp 
		${CMAKE_SOURCE_DIR}/filters/kpresenter/powerpoint/libppt/powerpoint.cpp 
		${CMAKE_SOURCE_DIR}/filters/kpresenter/powerpoint/libppt/ustring.cpp
		)

add_subdirectory( libppt )
add_subdirectory( import )




########### install files ###############






#original Makefile.am contents follow:

#SUBDIRS = libppt import
#
