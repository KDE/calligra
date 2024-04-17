set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${FLAKE_INCLUDES} )

# call: flake_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(FLAKE_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-flake-"
    )
endmacro()

########### next target ###############

flake_add_unit_test(TestAllowedInteraction TestAllowedInteraction.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestPosition TestPosition.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestSelection TestSelection.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestPathShape TestPathShape.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestPathTool TestPathTool.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestControlPointMoveCommand TestControlPointMoveCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestPointTypeCommand TestPointTypeCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestPointRemoveCommand TestPointRemoveCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestRemoveSubpathCommand TestRemoveSubpathCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestShapeAt TestShapeAt.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestShapePainting TestShapePainting.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestKoShapeFactory TestKoShapeFactory.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestKoShapeRegistry TestKoShapeRegistry.cpp  LINK_LIBRARIES flake Qt6::Test)

########### end ###############

flake_add_unit_test(TestShapeContainer TestShapeContainer.cpp  LINK_LIBRARIES flake Qt6::Test)

########### end ###############

flake_add_unit_test(TestShapeGroupCommand TestShapeGroupCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### end ###############

flake_add_unit_test(TestShapeReorderCommand TestShapeReorderCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestPathSegment TestPathSegment.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestImageCollection TestImageCollection.cpp  LINK_LIBRARIES flake Qt6::Test)

########### next target ###############

flake_add_unit_test(TestResourceManager TestResourceManager.cpp  LINK_LIBRARIES flake Qt6::Test)

########### end ###############

flake_add_unit_test(TestShapeBackgroundCommand TestShapeBackgroundCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### end ###############

flake_add_unit_test(TestShapeStrokeCommand TestShapeStrokeCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### end ###############

flake_add_unit_test(TestShapeShadowCommand TestShapeShadowCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### end ###############

flake_add_unit_test(TestSegmentTypeCommand TestSegmentTypeCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### end ###############

flake_add_unit_test(TestPointMergeCommand TestPointMergeCommand.cpp  LINK_LIBRARIES flake Qt6::Test)

########### end ###############

flake_add_unit_test(TestInputDevice TestInputDevice.cpp  LINK_LIBRARIES flake Qt6::Test)

########### end ###############

flake_add_unit_test(TestSnapStrategy TestSnapStrategy.cpp  LINK_LIBRARIES flake Qt6::Test)
