#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import traceback
import Kross
import Plan
import TestResult


TestResult.setResult( True )
asserttext1 = "Test of property '{0}' failed:\n   Expected: '{2}'\n     Result: '{1}'"
asserttext2 = "Failed to set property {0} to '{1}'. Result: {2}"

try:
    project = Plan.project()
    assert project is not None, "Project not found"

    group1 = project.createResourceGroup()
    assert group1 is not None, "Failed to create resource group"


    r1 = project.createResource( group1 )
    assert r1 is not None, "Could not create resource"

    r2 = project.createResource( group1 )
    assert r1 is not None, "Could not create resource"
    
    
    teamgroup = project.createResourceGroup()
    assert teamgroup is not None, "Failed to create resource group"

    team = project.createResource( teamgroup )
    assert r1 is not None, "Could not create resource"

    property = 'Type'
    data = "Team"
    res = project.setData(team, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    members = [ r1, r2 ]
    team.setChildren( members )
    newmembers = []
    for i in range( team.childCount() ):
        newmembers.append( team.childAt( i ) )

    assert newmembers == members, "Set team failed"

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
