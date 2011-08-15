#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import traceback
import Kross
import Plan
import TestResult


TestResult.setResult( True )
asserttext1 = "Test of property '{0}' failed:\n   Expected: '{2}'\n        Got: '{1}'"
asserttext2 = "Failed to set property '{0}' to '{1}'"

try:
    project = Plan.project()
    assert project is not None, "Project not found"
    
    group = project.createResourceGroup()
    assert group is not None, "Could not create resource group"
    project.addCommand( "Create resource group" );
    
    property = 'Name'
    data = "G1"
    before = project.data(group, property)
    res = project.setData(group, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(group, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(group, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'Type'
    data = "Material"
    before = project.data(group, property)
    res = project.setData(group, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(group, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    
    property = 'Type'
    data = "Work"
    res = project.setData(group, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(group, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(group, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    resource = project.createResource( group )
    assert group is not None, "Could not create resource"
    project.addCommand( "Create resource" );

    property = 'Name'
    data = "R1"
    before = project.data(resource, property)
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'Type'
    data = "Material"
    before = project.data(resource, property)
    
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = "Work"
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = "Team"
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text

    project.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'Initials'
    data = "RR"
    before = project.data(resource, property)
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'Email'
    data = "R1@work.org"
    before = project.data(resource, property)
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'Email'
    data = "R1@work.org"
    before = project.data(resource, property)
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

#        ResourceCalendar,

    property = 'Limit'
    data = 10
    before = project.data(resource, property)
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'AvailableFrom'
    data = "2011-07-01T08:00:00"
    before = project.data(resource, property, 'EditRole', -1)
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property, 'EditRole', -1)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(resource, property, 'EditRole', -1)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'AvailableUntil'
    data = "2011-07-02T08:00:00"
    before = project.data(resource, property, 'EditRole', -1)
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property, 'EditRole', -1)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(resource, property, 'EditRole', -1)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'NormalRate'
    data = 111
    before = project.data(resource, property, 'EditRole', -1)
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property, 'EditRole', -1)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(resource, property, 'EditRole', -1)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'OvertimeRate'
    data = 222
    before = project.data(resource, property, 'EditRole', -1)
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data)
    assert res == True, text
    result = project.data(resource, property, 'EditRole', -1)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(resource, property, 'EditRole', -1)
    text = asserttext1.format(property, result, before)

#ResourceAccount

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
