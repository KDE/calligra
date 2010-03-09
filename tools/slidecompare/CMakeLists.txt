project (ooslideview)
cmake_minimum_required(VERSION 2.6)
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(OOoSDK REQUIRED)
find_package(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL 1)
include(${QT_USE_FILE})

include_directories(
	${OOOSDK_INCLUDE_DIRS}
	${KOMAIN_INCLUDES}
	${KOPAGEAPP_INCLUDES}
	../../filters/kpresenter/powerpoint
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fPIC")

add_executable(ooslidecreate ooslidecreate.cpp)
target_link_libraries(ooslidecreate ${OOOSDK_LIBRARIES} ${QT_LIBRARIES})

QT4_WRAP_CPP(MOCFILES slideview.h combinedview.h slideloader.h dirslideloader.h oothread.h)

add_executable(ooslideview slideview.cpp combinedview.cpp ooslideview.cpp
	kpresenterslideloader.cpp oothread.cpp ${MOCFILES})
target_link_libraries(ooslideview ${OOOSDK_LIBRARIES} ${QT_LIBRARIES}
	komain kopageapp ppttoodplib)
