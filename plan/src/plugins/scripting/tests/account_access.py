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
    
    account = project.createAccount( 0 )
    assert account is not None, "Could not create account"

    property = 'Name'
    data = "Account name"
    before = account.name()
    Plan.beginCommand("Set data")
    res = project.setData(account, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = account.name()
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = account.name()
    text = asserttext.format(property, result, before)
    assert result == before, text

    property = 'Name'
    data = "Account name"
    before = project.data(account, property)
    Plan.beginCommand("Set data")

    res = project.setData(account, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(account, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(account, property)
    text = asserttext.format(property, result, before)
    assert result == before, text
    
    property = 'Description'
    data = "Account description"
    before = project.data(account, property)
    Plan.beginCommand("Set data")
    res = project.setData(account, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    result = project.data(account, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    Plan.revertCommand()
    result = project.data(account, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
