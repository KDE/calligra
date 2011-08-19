#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import traceback
import Kross
import Plan
import TestResult


TestResult.setResult( True )
asserttext = "Test of property '{0}' failed:\n   Expected: '{2}'\n        Got: '{1}'"
asserttext2 = "Test of setting {0} to {1} failed"

try:
    project = Plan.project()
    assert project is not None
    
    calendar = project.createCalendar( 0 )
    assert calendar is not None, "Could not create calendar"
    project.addCommand( "Create calendar" );

    property = 'Name'
    data = "Calendar name"
    before = project.data(calendar, property)
    res = project.setData(calendar, property, data)
    assert res == True
    result = project.data(calendar, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(calendar, property)
    text = asserttext.format(property, result, before)
    assert result == before, text
    
    property = 'Name'
    data = 2 # Checked
    before = project.data(calendar, property)
    res = project.setData(calendar, property, data, 'CheckStateRole')
    assert res == True, "setData failed"
    result = project.data(calendar, property, 'CheckStateRole', -1)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(calendar, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

    property = 'Weekday'
    data = ['Monday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text

    data = ['Tuesday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text

    data = ['Wednesday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text

    data = ['Thursday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text

    data = ['Friday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text

    data = ['Saturday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text

    data = ['Sunday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text
    data = ['Sunday', 'NonWorking']
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text
    data = ['Sunday', 'Undefined']
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text

    property = 'Date'
    data = ["2011-08-21", "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text

    property = 'Noday'
    data = ["08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == False, text

    property = 'Date'
    data = ["08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == False, text

    data = ["2011-01-01", "NonWorking"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text

    data = ["2011-01-01", "Undefined"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data)
    assert res == True, text

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
