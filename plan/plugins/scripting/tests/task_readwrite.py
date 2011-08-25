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
    assert project is not None, "Project not found"
    
    task1 = project.createTask( 0 )
    assert task1 is not None, "Could not create task"
    
    task2 = project.createTask( 0 )
    assert task2 is not None, "Could not create task"

    data = "Task name"
    property = 'Name'
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text1
    result = project.data(task1, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    
    data = "Task responsible"
    property = 'Responsible'
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Allocation'
    data = "John Doe,Jane Doe"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'DisplayRole' )
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Estimate'
    data = "3.0d"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'DisplayRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'EstimateType'
    data = "Duration"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'DisplayRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'EstimateType'
    data = "Effort"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'DisplayRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Constraint'
    data = "ALAP"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Constraint'
    data = "ASAP"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Constraint'
    data = "StartNotEarlier"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Constraint'
    data = "MustStartOn"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Constraint'
    data = "FinishNotLater"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Constraint'
    data = "MustFinishOn"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Constraint'
    data = "FixedInterval"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text
    
    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'ConstraintStart'
    data = "2011-08-01T10:00:00"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'ConstraintEnd'
    data = "2011-08-01T11:00:00"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'OptimisticRatio'
    data = -20
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'PessimisticRatio'
    data = 120
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Constraint'
    data = "ASAP"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    property = 'Estimate'
    data = "4.0w"
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'DisplayRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Risk'
    data = 'Low'
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    property = 'Risk'
    data = 'High'
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Risk'
    data = 'None'
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'StartupCost'
    data = 1000.00
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'ShutdownCost'
    data = 1000.00
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Description'
    data = 'Task description'
    res = project.setData(task1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(task1, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = project.data(task1, property, 'ProgramRole')
    res = project.setData(task2, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
