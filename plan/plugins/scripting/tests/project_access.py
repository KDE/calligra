#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import traceback
import Kross
import Plan
import TestResult


TestResult.setResult( True )
asserttext = "Test of property '{0}' failed:\n   Expected: '{2}'\n     Result: '{1}'"
asserttext2 = "Failed to set property '{0}' to '{1}'. Result: {2}"

try:
    project = Plan.project()
    assert project is not None
    
    property = 'Name'
    data = "Project name"
    before = project.name()
    Plan.beginCommand( "Set data" );
    res = project.setData(project, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.name()
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.name()
    text = asserttext.format(property, result, before)
    assert result == before, text

    property = 'Name'
    data = "Project name"
    before = project.data(project, property)
    Plan.beginCommand( "Set data" );
    res = project.setData(project, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(project, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

    property = 'Responsible'
    data = "Project responsible"
    before = project.data(project, property)
    Plan.beginCommand( "Set data" );
    res = project.setData(project, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(project, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

    property = 'ConstraintStart'
    data = "2011-08-01T10:00:00"
    before = project.data(project, property)
    Plan.beginCommand( "Set data" );
    res = project.setData(project, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(project, property, 'ProgramRole')
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

    property = 'ConstraintEnd'
    data = "2011-08-01T11:00:00"
    before = project.data(project, property)
    Plan.beginCommand( "Set data" );
    res = project.setData(project, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(project, property, 'ProgramRole')
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
