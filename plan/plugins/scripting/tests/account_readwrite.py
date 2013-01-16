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
    
    account = project.createAccount( 0 )
    assert account is not None, "Could not create account"

    property = 'Name'
    data = "Account name"
    res = project.setData(account, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text
    
    property = 'Description'
    data = "Account description"
    res = project.setData(account, property, data)
    text = asserttext2.format(property, data, res)
    assert res == 'Success', text


    a2 = project.createAccount( 0 )
    assert a2 is not None, "Could not create account"

    props = project.accountPropertyList()
    for p in props:
        data = project.data(account, p, 'ProgramRole')
        res = project.setData(a2, p, data)
        if res != 'ReadOnly':
            text = asserttext2.format(property, data, res)
            assert res == 'Success', text
        
except:
    TestResult.setResult( False )
    TestResult.setMessage("\n" + traceback.format_exc(1))
