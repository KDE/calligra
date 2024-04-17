set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")

include_directories( ${PIGMENT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/../colorspaces )

if(MSVC OR (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
  # avoid "cannot open file 'LIBC.lib'" error
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
endif()

# call: pigment_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(PIGMENT_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-pigment-"
    )
endmacro()

########### next target ###############

pigment_add_unit_test(TestKoColorSpaceRegistry TestKoColorSpaceRegistry.cpp  LINK_LIBRARIES pigmentcms KF6::I18n Qt6::Test)

########### next target ###############

pigment_add_unit_test(TestColorConversion TestColorConversion.cpp  LINK_LIBRARIES pigmentcms Qt6::Test)

########### next target ###############

pigment_add_unit_test(TestKoColorSpaceAbstract TestKoColorSpaceAbstract.cpp  LINK_LIBRARIES pigmentcms KF6::I18n Qt6::Test)

########### next target ###############

pigment_add_unit_test(TestKoColorSpaceMaths TestKoColorSpaceMaths.cpp  LINK_LIBRARIES pigmentcms Qt6::Test)

########### next target ###############
set(CCSGraph_GRAPH CCSGraph.cpp)
add_executable(CCSGraph ${CCSGraph_GRAPH})
ecm_mark_as_test(CCSGraph)
target_link_libraries(CCSGraph  pigmentcms KF6::I18n )

########### next target ###############

pigment_add_unit_test(TestColorConversionSystem TestColorConversionSystem.cpp  LINK_LIBRARIES pigmentcms KF6::I18n Qt6::Test)

########### next target ###############

pigment_add_unit_test(TestKoColor TestKoColor.cpp  LINK_LIBRARIES pigmentcms KF6::I18n Qt6::Test)

########### next target ###############

pigment_add_unit_test(TestKoIntegerMaths TestKoIntegerMaths.cpp  LINK_LIBRARIES pigmentcms KF6::I18n Qt6::Test)

########### next target ###############

pigment_add_unit_test(TestConvolutionOpImpl TestConvolutionOpImpl.cpp  LINK_LIBRARIES pigmentcms KF6::I18n Qt6::Test)

########### next target ###############

pigment_add_unit_test(TestKoRgbU8ColorSpaceTester KoRgbU8ColorSpaceTester.cpp  LINK_LIBRARIES pigmentcms KF6::I18n Qt6::Test)

########### next target ###############

pigment_add_unit_test(TestKoColorSpaceSanity TestKoColorSpaceSanity.cpp  LINK_LIBRARIES pigmentcms KF6::I18n Qt6::Test)

########### next target ###############

pigment_add_unit_test(TestFallBackColorTransformation TestFallBackColorTransformation.cpp  LINK_LIBRARIES pigmentcms KF6::I18n Qt6::Test)

########### next target ###############

pigment_add_unit_test(TestKoChannelInfo TestKoChannelInfo.cpp  LINK_LIBRARIES pigmentcms KF6::I18n Qt6::Test)
