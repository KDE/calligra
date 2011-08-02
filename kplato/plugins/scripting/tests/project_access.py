#!/usr/bin/env kross
# -*- coding: utf-8 -*-

from datetime import datetime
import traceback
import Kross
import Plan
import TestResult


TestResult.setResult( True )
asserttext = "Test of property '{0}' failed: '{1}' != '{2}'"
try:
    project = Plan.project()
    assert project is not None
    
    data = "Project name"
    property = 'Name'
    before = project.data(project, property)
    project.setData(project, property, data)
    result = project.data(project, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text
    
    data = "Project responsible"
    property = 'Responsible'
    before = project.data(project, property)
    project.setData(project, property, data)
    result = project.data(project, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

    data = "2011-08-01T10:00:00"
    property = 'ConstraintStart'
    before = project.data(project, property)
    project.setData(project, property, data)
    result = project.data(project, property, 'Qt::EditRole', -1)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

    data = "2011-08-01T11:00:00"
    property = 'ConstraintEnd'
    before = project.data(project, property)
    project.setData(project, property, data)
    result = project.data(project, property, 'Qt::EditRole', -1)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(project, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
