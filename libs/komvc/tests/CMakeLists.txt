include_directories(${KOMVC_INCLUDES})

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

set(mvctest_SRCS 
    main.cpp
    TestApplication.cpp
    TestDocument.cpp
    TestMainWindow.cpp
    TestView.cpp
)
kde4_add_executable(mvctest ${mvctest_SRCS})
target_link_libraries(mvctest komvc)
