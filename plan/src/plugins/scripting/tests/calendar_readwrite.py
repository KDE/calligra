#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import traceback
import Kross
import Plan
import TestResult


TestResult.setResult( True )
asserttext1 = "Test of property '{0}' failed:\n   Expected: '{2}'\n     Result: '{1}'"
asserttext2 = "Failed to set property '{0}' to '{1}'. Result: {2}"

try:
    project = Plan.project()
    assert project is not None
    
    calendar = project.createCalendar( 0 )
    assert calendar is not None, "Could not create calendar"

    property = 'Name'
    data = "Calendar name"
    res = project.setData(calendar, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    
    property = 'Name'
    data = 2 # Checked
    res = project.setData(calendar, property, data, 'CheckStateRole')
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text + " role = CheckStateRole"

    c2 = project.createCalendar( 0 )
    assert c2 is not None, "Could not create calendar"

    props = project.calendarPropertyList()
    for p in props:
        data = project.data(calendar, p, 'ProgramRole')
        res = project.setData(c2, p, data)
        if res != 'ReadOnly':
            text = asserttext2.format(property, data, res)
            assert res == 'Success', text
        
except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
