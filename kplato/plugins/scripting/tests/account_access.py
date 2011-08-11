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
    
    account = project.createAccount( 0 )
    assert account is not None, "Could not create account"
    project.addCommand( "Create account" );

    property = 'Name'
    data = "Account name"
    before = project.data(account, property)
    project.setData(account, property, data)
    result = project.data(account, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(account, property)
    text = asserttext.format(property, result, before)
    assert result == before, text
    
    property = 'Description'
    data = "Account description"
    before = project.data(account, property)
    project.setData(account, property, data)
    result = project.data(account, property)
    text = asserttext.format(property, result, data)
    assert result == data, text
    project.revertCommand()
    result = project.data(account, property)
    text = asserttext.format(property, result, before)
    assert result == before, text

except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
