add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ../colorspaces/cmyk_u16
    ../colorspaces/cmyk_u8
    ../colorspaces/gray_u16
    ../colorspaces/gray_u8
    ../colorspaces/lab_u16
    ../colorspaces/rgb_u16
    ../colorspaces/rgb_u8
    ../colorspaces/xyz_u16
    ../colorprofiles
    ..
)
if(OpenEXR_FOUND)
    include_directories(${OPENEXR_INCLUDE_DIR})
endif()
include_directories( ${PIGMENT_INCLUDES} ${LCMS2_INCLUDE_DIR} )


if(MSVC OR (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
  # avoid "cannot open file 'LIBC.lib'" error
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
endif()

# call: colorengine_lcms_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(COLORENGINE_LCMS_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "colorengine-lcms-"
    )
endmacro()

########### next target ###############

colorengine_lcms_add_unit_test(TestKoLcmsColorProfile
    TestKoLcmsColorProfile.cpp
    LINK_LIBRARIES kowidgets pigmentcms KF6::I18n ${LCMS2_LIBRARIES} Qt6::Test
)

########### next target ###############

colorengine_lcms_add_unit_test(TestKoLcmsColorSpaceRegistry
    TestKoLcmsColorSpaceRegistry.cpp
    LINK_LIBRARIES kowidgets pigmentcms KF6::I18n ${LCMS2_LIBRARIES} Qt6::Test
)

########### next target ###############

# colorengine_lcms_add_unit_test(TestKoCompositeOps
#     TestKoCompositeOps.cpp
#     LINK_LIBRARIES kowidgets pigmentcms Qt6::Test
# )


