#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import traceback
import Kross
import Plan
import TestResult


TestResult.setResult( True )
asserttext = "Test of property '{0}' failed:\n   Expected: '{2}'\n        Got: '{1}'"

try:
    project = Plan.project()
    assert project is not None
    
    property = 'Name'
    data = "Project name"
    before = project.data(project, property)
    project.setData(project, property, data)
    result = project.data(project, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text
    
    property = 'Responsible'
    data = "Project responsible"
    before = project.data(project, property)
    project.setData(project, property, data)
    result = project.data(project, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

    property = 'ConstraintStart'
    data = "2011-08-01T10:00:00"
    before = project.data(project, property)
    project.setData(project, property, data)
    result = project.data(project, property, 'EditRole', -1)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

    property = 'ConstraintEnd'
    data = "2011-08-01T11:00:00"
    before = project.data(project, property)
    project.setData(project, property, data)
    result = project.data(project, property, 'EditRole', -1)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
