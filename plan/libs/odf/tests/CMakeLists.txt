set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${PLANODF_INCLUDES} )

# call: planodf_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(PLANODF_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "Plan${_TEST_NAME}"
        NAME_PREFIX "plan-odf-"
    )
endmacro()

########### next target ###############

planodf_add_unit_test(TestKoGenStyles TestKoGenStyles.cpp  LINK_LIBRARIES planodf KF5::I18n Qt5::Test)

########### next target ###############

planodf_add_unit_test(TestOdfSettings TestOdfSettings.cpp  LINK_LIBRARIES planodf KF5::I18n Qt5::Test)

########### next target ###############

planodf_add_unit_test(TestKoOdfLoadingContext TestKoOdfLoadingContext.cpp  LINK_LIBRARIES planodf KF5::I18n Qt5::Test)

########### next target ###############

planodf_add_unit_test(TestXmlWriter TestXmlWriter.cpp  LINK_LIBRARIES planodf Qt5::Test)

########### next target ###############

planodf_add_unit_test(TestXmlReader TestXmlReader.cpp  LINK_LIBRARIES planodf Qt5::Test)

########### next target ###############

planodf_add_unit_test(TestXmlReaderWithoutSpaces TestXmlReaderWithoutSpaces.cpp  LINK_LIBRARIES planodf Qt5::Test)

########### next target ###############

planodf_add_unit_test(kodomtest kodomtest.cpp  LINK_LIBRARIES planodf Qt5::Test)

########### next target ###############

planodf_add_unit_test(TestStorage TestStorage.cpp  LINK_LIBRARIES planodf KF5::I18n Qt5::Test)

########### next target ###############

planodf_add_unit_test(TestKoUnit TestKoUnit.cpp  LINK_LIBRARIES planodf Qt5::Test)

########### next target ###############

planodf_add_unit_test(TestNumberStyle TestNumberStyle.cpp  LINK_LIBRARIES planodf Qt5::Test)

########### next target ###############

planodf_add_unit_test(TestKoElementReference TestKoElementReference.cpp  LINK_LIBRARIES planodf Qt5::Test)

########### next target ###############

planodf_add_unit_test(TestWriteStyleXml TestWriteStyleXml.cpp  LINK_LIBRARIES planodf Qt5::Test)

########### end ###############
