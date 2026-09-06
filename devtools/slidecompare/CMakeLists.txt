find_package(OOoSDK)

if(OOOSDK_FOUND)
	include_directories(
		${OOOSDK_INCLUDE_DIRS}
		${KOMAIN_INCLUDES}
		${KOPAGEAPP_INCLUDES}
                ../../filters/stage/powerpoint
		../../filters/libmso
	)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

	qt_wrap_cpp(MOCFILES slideview.h combinedview.h slideloader.h
		dirslideloader.h oothread.h)

	add_executable(slidecompare
		slideview.cpp combinedview.cpp slidecompare.cpp
		kpresenterslideloader.cpp oothread.cpp ${MOCFILES})
	set_property(TARGET slidecompare PROPERTY POSITION_INDEPENDENT_CODE ON)
	target_link_libraries(slidecompare ${OOOSDK_LIBRARIES} ${QT_LIBRARIES}
		Qt6::OpenGL komain kopageapp ppttoodplib)
	# it would be nice if this explicit dependency was not needed
	add_dependencies(slidecompare _oooheaders)
endif()
