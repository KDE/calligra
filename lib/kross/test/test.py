#!/usr/bin/env python

"""
  This Python script is used to test the Kross scripting framework.
"""

class testClass:
    i = 123;
    s = "string-attribute from testClass"
    def __init__(self):
        print "testKexiDBClass Constructor."
    def testClassFunction1(args):
        print "testClassFunction1() called in pythonscript."
        return "testClassFunction1() returnvalue"
    def testClassFunction2(args):
        print "testClassFunction2() called in pythonscript."
        return "testClassFunction2() returnvalue"

def testfunc(msg):
    global globalvar
    globalvar = globalvar + 1
    print "testfunc() returnvalue msg='%s' globalvar='%s'" % (msg,globalvar)
    return "this is the __main__.testfunc() returnvalue!"

#def testobjectCallback():
#    print "function testobjectCallback() called !"
#    return "this is the __main__.testobjectCallback() returnvalue!"
#def testobjectCallbackWithParams(argument):
#    print "testobjectCallbackWithParams() argument = %s" % str(argument)
#    return "this is the __main__.testobjectCallbackWithParams() returnvalue!"

def testQtObject(self):

    # Get the QtObject instance to access the QObject.
    #testobject = get("TestObject")
    testobject = self.get("TestObject")
    if testobject == None:
        raise "Object 'TestObject' undefined !!!"

    print "testobject = %s %s" % (str(testobject),dir(testobject))
    #print "propertyNames = %s" % testobject.propertyNames()
    #print "slotNames = %s" % testobject.slotNames()
    #print "signalNames = %s" % testobject.signalNames()

    # We could just call a slot or a signal.
    print "################################### 1"
    print testobject.call("testSlot2()");
    print testobject.call("testSignal()");
    #print testobject.call() #KrossTest: List::item index=0 is out of bounds. Raising TypeException.

    # Each slot a QObject spends is a object itself.
    print "################################### 2"
    myslot = testobject.get("testSlot()")
    print "myslotevent = %s" % str(myslot)
    print "################################### 3"
    print myslot.call()

    print "################################### 4"
    print "__name__ = %s" % __name__
    print "__dir__ = %s" % dir()
    #print "__builtin__ = %s" % __builtin__
    print "self = %s %s" % (str(self),dir(self))
    #print "TestCase = %s" % str(TestCase)

    print "################################### 5"
    print "self.list = %s" % self.list()
    print "self.dict = %s" % self.dict()
    print "################################### 6"
    testobject = self.get("TestObject")
    print "testobject = %s" % testobject

    print "################################### 7"
    if not testobject.connect("testSignal()",testobject,"testSlot2()"):
        raise "Failed to connect testSignal() with testSlot2() at object 'TestObject'."
    testobject.signal("testSignal()")
    #testobject.testSlot()
    testobject.slot("testSlot()")
    testobject.disconnect("testSignal()")

def testActionEvent(self):

    #action1 = get("Action1")
    action1 = self.get("Action1")
    if action1 == None:
        raise "Object 'Action1' undefined !!!"

    print "action1 = %s %s" % (str(action1),dir(action1))

    print "################################### 1"
    #action1.call()
    action1.activate()

#def testPluginModuleCallback(arg1, arg2 = None):
#    print "testPluginModuleCallback => arg1='%s' arg2='%s'" % (arg1,arg2)
#    return "testPluginModuleCallback_returnvalue"

def testPluginModule():
    import krosstestpluginmodule
    print "krosstestpluginmodule => %s %s" % (krosstestpluginmodule,dir(krosstestpluginmodule))

    testobject1 = krosstestpluginmodule.testpluginobject1()
    print "testpluginobject1 => %s %s" % (testobject1,dir(testobject1))

    print "-------------------------------------"
    print testobject1.func1()
    print "-------------------------------------"
    print testobject1.func2("func2string1","func2string2")
    print "-------------------------------------"
    print testobject1.func3("func3string1","func3string2")
    print "-------------------------------------"
    print testobject1.func3("func3string1")
    print "-------------------------------------"
    print testobject1.func4("func4string1","func4string2")
    print "-------------------------------------"
    print testobject1.func5("func5string1","func5string2")
    print "-------------------------------------"

    #print "-------------------------------------"
    #print testobject1.overloadedFunc()
    #print "-------------------------------------"
    #print testobject1.overloadedFunc("somestring")
    #print "-------------------------------------"
    #print testobject1.overloadedFunc(22)
    #print "-------------------------------------"
    #print testobject1.overloadedFunc(True)
    #print "-------------------------------------"
    #print testobject1.overloadedFunc("somestring1","somestring2")
    #print "-------------------------------------"
    #print testobject1.overloadedFunc("somestring",22)

print "########## BEGIN TEST: testpluginmodule ##########"
testPluginModule()
print "########## END TEST: testpluginmodule ##########"

#print "########## BEGIN TEST: QObject ##########"
#testQtObject(self)
#print "########## END TEST: QObject ##########"

#print "########## BEGIN TEST: ActionEvent ##########"
#testActionEvent(self)
#print "########## END TEST: ActionEvent ##########"

#testfunc("from __main__")
#maintestfunc()
#print __name__
