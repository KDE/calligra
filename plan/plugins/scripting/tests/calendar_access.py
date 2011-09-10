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
    
    calendar = project.createCalendar( 0 )
    assert calendar is not None, "Could not create calendar"

    property = 'Name'
    data = "Calendar name"
    before = calendar.name()
    Plan.beginCommand("Set data")
    res = project.setData(calendar, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = calendar.name()
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = calendar.name()
    text = asserttext.format(property, result, before)
    assert result == before, text

    property = 'Name'
    data = "Calendar name"
    before = project.data(calendar, property)
    Plan.beginCommand("Set data")
    res = project.setData(calendar, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(calendar, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(calendar, property)
    text = asserttext.format(property, result, before)
    assert result == before, text
    
    property = 'Name'
    data = 2 # Checked
    before = project.data(calendar, property)
    Plan.beginCommand("Set data")
    res = project.setData(calendar, property, data, 'CheckStateRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text + " role = CheckStateRole"
    result = project.data(calendar, property, 'CheckStateRole', -1)
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(calendar, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

    property = 'Weekday'
    data = ['Monday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    data = ['Tuesday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    data = ['Wednesday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    data = ['Thursday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    data = ['Friday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    data = ['Saturday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    data = ['Sunday', "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    data = ['Sunday', 'NonWorking']
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    data = ['Sunday', 'Undefined']
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Date'
    data = ["2011-08-21", "08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Noday'
    data = ["08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res != 'Success', text

    property = 'Date'
    data = ["08:30:00", "12:00:00"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res != 'Success', text

    data = ["2011-01-01", "NonWorking"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    data = ["2011-01-01", "Undefined"]
    res = project.setData(calendar, property, data, 'EditRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
