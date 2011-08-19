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
    
    group = project.createResourceGroup()
    assert group is not None, "Failed to create resource group"
    
    #Resource group does not have a name() method: should it?
    #property = 'Name'
    #data = "G1"
    #before = group.name()
    #Plan.beginCommand("Set data")
    #res = project.setData(group, property, data)
    #text = asserttext2.format(property, data, res)
    #assert res == 'Success', text
    #result = group.name()
    #text = asserttext1.format(property, result, data)
    #assert result == data, text
    #Plan.revertCommand()
    #result = group.name()
    #text = asserttext1.format(property, result, before)
    #assert result == before, text

    property = 'Name'
    data = "G1"
    before = project.data(group, property)
    Plan.beginCommand("Set data")
    res = project.setData(group, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(group, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(group, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'Type'
    data = "Material"
    before = project.data(group, property)
    Plan.beginCommand("Set data")
    res = project.setData(group, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(group, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    
    property = 'Type'
    data = "Work"
    res = project.setData(group, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(group, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(group, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    resource = project.createResource( group )
    assert resource is not None, "Failed to create resource"

    #Should resource have name() method?
    #property = 'Name'
    #data = "R1"
    #before = resource.name()
    #Plan.beginCommand("Set data")
    #res = project.setData(resource, property, data)
    #text = asserttext2.format(property, data, res)
    #assert res == 'Success', text
    #result = resource.name()
    #text = asserttext1.format(property, result, data)
    #assert result == data, text
    #Plan.revertCommand()
    #result = resource.name()
    #text = asserttext1.format(property, result, before)
    #assert result == before, text

    property = 'Name'
    data = "R1"
    before = project.data(resource, property)
    Plan.beginCommand("Set data")
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'Type'
    data = "Material"
    before = project.data(resource, property)
    Plan.beginCommand("Set data")
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = "Work"
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text

    data = "Team"
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text

    Plan.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'Initials'
    data = "RR"
    before = project.data(resource, property)
    Plan.beginCommand("Set data")
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'Email'
    data = "R1@work.org"
    before = project.data(resource, property)
    Plan.beginCommand("Set data")
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'Email'
    data = "R1@work.org"
    before = project.data(resource, property)
    Plan.beginCommand("Set data")
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

#        ResourceCalendar,

    property = 'Limit'
    data = 10
    before = project.data(resource, property)
    Plan.beginCommand("Set data")
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property)
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(resource, property)
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'AvailableFrom'
    data = "2011-07-01T08:00:00"
    before = project.data(resource, property, 'EditRole')
    Plan.beginCommand("Set data")
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(resource, property, 'EditRole')
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'AvailableUntil'
    data = "2011-07-02T08:00:00"
    before = project.data(resource, property, 'EditRole')
    Plan.beginCommand("Set data")
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(resource, property, 'EditRole')
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'NormalRate'
    data = 111
    before = project.data(resource, property, 'EditRole')
    Plan.beginCommand("Set data")
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(resource, property, 'EditRole')
    text = asserttext1.format(property, result, before)
    assert result == before, text

    property = 'OvertimeRate'
    data = 222
    before = project.data(resource, property, 'EditRole')
    Plan.beginCommand("Set data")
    res = project.setData(resource, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property, 'EditRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(resource, property, 'EditRole')
    text = asserttext1.format(property, result, before)

    account = project.createAccount(0)
    data = 'A1'
    assert account is not None, "Failed to create account"
    res = project.setData(account, 'Name', data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(account, 'Name')
    text = asserttext1.format(property, result, data)
    assert result == data, text
    
    property = 'Account'
    res = project.setData(resource, property, data) 
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(resource, property, 'ProgramRole')
    text = asserttext1.format(property, result, data)
    assert result == data, text

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
