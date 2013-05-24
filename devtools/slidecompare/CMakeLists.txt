find_package(OOoSDK)

if(OOOSDK_FOUND)
	include_directories(
		${OOOSDK_INCLUDE_DIRS}
		${KOMAIN_INCLUDES}
		${KOPAGEAPP_INCLUDES}
                ../../filters/stage/powerpoint
		../../filters/libmso
	)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS} -fPIC")

	QT4_WRAP_CPP(MOCFILES slideview.h combinedview.h slideloader.h
		dirslideloader.h oothread.h)

	add_executable(slidecompare
		slideview.cpp combinedview.cpp slidecompare.cpp
		kpresenterslideloader.cpp oothread.cpp ${MOCFILES})
	target_link_libraries(slidecompare ${OOOSDK_LIBRARIES} ${QT_LIBRARIES}
		${QT_QTOPENGL_LIBRARY} komain kopageapp ppttoodplib)
	# it would be nice if this explicit depencency was not needed
	add_dependencies(slidecompare _oooheaders)
endif(OOOSDK_FOUND)
