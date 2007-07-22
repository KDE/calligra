######### Test for Hello #########
set(TestHello_SRCS TestHello.cpp ../Hello.cpp ../User.cpp ../Generic.cpp XmlHelper.cpp )
kde4_add_executable(TestHello NOGUI RUN_UNINSTALLED ${TestHello_SRCS})
target_link_libraries(TestHello ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
add_test(collaborationMessages-Hello ${EXECUTABLE_OUTPUT_PATH}/TestHello)

#Don't start compilation of test for some reason
#kde4_add_unit_test(TestHello TESTNAME collaborationMessages-Hello  ${TestHello_SRCS})
#target_link_libraries(TestHello ${QT_QTTEST_LIBRARY})

######### Test for User #########
set(TestUser_SRCS TestUser.cpp ../User.cpp ../Generic.cpp XmlHelper.cpp )
kde4_add_executable(TestUser NOGUI RUN_UNINSTALLED ${TestUser_SRCS})
target_link_libraries(TestUser ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
add_test(collaborationMessages-User ${EXECUTABLE_OUTPUT_PATH}/TestUser)

######### Test for HelloAnswer #########
set(TestHelloAnswer_SRCS TestHelloAnswer.cpp ../HelloAnswer.cpp ../Generic.cpp XmlHelper.cpp )
kde4_add_executable(TestHelloAnswer NOGUI RUN_UNINSTALLED ${TestHelloAnswer_SRCS})
target_link_libraries(TestHelloAnswer ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
add_test(collaborationMessages-HelloAnswer ${EXECUTABLE_OUTPUT_PATH}/TestHelloAnswer)

######### Test for Update #########
set(TestUpdate_SRCS TestUpdate.cpp ../Update.cpp ../Generic.cpp XmlHelper.cpp )
kde4_add_executable(TestUpdate NOGUI RUN_UNINSTALLED ${TestUpdate_SRCS})
target_link_libraries(TestUpdate ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
add_test(collaborationMessages-Update ${EXECUTABLE_OUTPUT_PATH}/TestUpdate)

######### Test for UpdateAnswer #########
set(TestUpdateAnswer_SRCS TestUpdateAnswer.cpp ../UpdateAnswer.cpp ../Generic.cpp XmlHelper.cpp )
kde4_add_executable(TestUpdateAnswer NOGUI RUN_UNINSTALLED ${TestUpdateAnswer_SRCS})
target_link_libraries(TestUpdateAnswer ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
add_test(collaborationMessages-UpdateAnswer ${EXECUTABLE_OUTPUT_PATH}/TestUpdateAnswer)

######### Test for MessageFactory #########
set(TestMessageFactory_SRCS
    TestMessageFactory.cpp ../MessageFactory.cpp ../Generic.cpp XmlHelper.cpp
    ../Hello.cpp ../User.cpp ../HelloAnswer.cpp ../Update.cpp ../UpdateAnswer.cpp
)
kde4_add_executable(TestMessageFactory NOGUI RUN_UNINSTALLED ${TestMessageFactory_SRCS})
target_link_libraries(TestMessageFactory ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
add_test(collaborationMessages-MessageFactory ${EXECUTABLE_OUTPUT_PATH}/TestMessageFactory)
