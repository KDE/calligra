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
    
    group1 = project.createResourceGroup()
    assert group1 is not None, "Failed to create resource group"

    property = 'Name'
    data = "G1"
    res = project.setData(group1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(group1, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text

    property = 'Type'
    data = "Material"
    res = project.setData(group1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(group1, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text

    group2 = project.createResourceGroup()
    assert group2 is not None, "Failed to create resource group"

    props = project.resourcePropertyList()
    for p in props:
        data = project.data(group1, p, 'ProgramRole')
        res = project.setData(group2, p, data)
        if res != 'ReadOnly':
            text = asserttext2.format(property, data, res)
            assert res == 'Success', text
    
    
    r1 = project.createResource( group1 )
    assert r1 is not None, "Could not create resource"

    property = 'Name'
    data = "R1"
    res = project.setData(r1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Type'
    data = "Material"
    before = project.data(r1, property)
    
    res = project.setData(r1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Initials'
    data = "RR"
    res = project.setData(r1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Email'
    data = "R1@work.org"
    res = project.setData(r1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Email'
    data = "R1@work.org"
    res = project.setData(r1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'Limit'
    data = 10
    res = project.setData(r1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'AvailableFrom'
    data = "2011-07-01T08:00:00"
    res = project.setData(r1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'AvailableUntil'
    data = "2011-07-02T08:00:00"
    res = project.setData(r1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'NormalRate'
    data = 111
    res = project.setData(r1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    property = 'OvertimeRate'
    data = 222
    res = project.setData(r1, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    account = project.createAccount(0)
    data = 'A1'
    assert account is not None, "Failed to create account"
    res = project.setData(account, 'Name', data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    
    property = 'Account'
    res = project.setData(r1, property, data) 
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text

    r2 = project.createResource( group2 )
    assert r2 is not None, "Could not create resource"

    for p in props:
        data = project.data(r1, p, 'ProgramRole')
        res = project.setData(r2, p, data)
        if res != 'ReadOnly':
            text = asserttext2.format(property, data, res)
            assert res == 'Success', text

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
