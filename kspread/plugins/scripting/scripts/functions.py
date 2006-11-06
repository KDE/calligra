#!/usr/bin/env kross

"""
Python script that provides additional formula-functions
to KSpread.

The script could be used in two ways;

    1. Embedded in KSpread by execution via the "Tools=>Scripts"
       menu or from the "Tools=>Script Manager". The functions.py
       script may not be visible if not installed. So, for testing
       better use the second way;

    2. Run KSpread with;

            # make the script executable
            chmod 755 `kde4-config --install data`/kspread/scripts/functions/functions.py
            # run KSpread
            kspread --scriptfile `kde4-config --install data`/kspread/scripts/functions/functions.py

(C)2006 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.koffice.org/kspread
Dual-licensed under LGPL v2+higher and the BSD license.
"""

class Setup:

    def __init__(self, scriptaction):
        import os, sys

        try:
            import Kross
        except:
            raise "Failed to import the Kross module."

        self.scriptaction = scriptaction
        #self.currentpath = self.scriptaction.currentPath()

        self.embeddedInKSpread = False
        try:
            import KSpread
            self.kspread = KSpread
            self.embeddedInKSpread = True
        except ImportError:
            try:
                import Kross
                self.kspread = Kross.module("kspread")
            except ImportError:
                raise "Failed to import the KSpread Kross module. Please run this script with \"kross thisscriptfile.py\""

        self.addTestFunctions()

    def addTestFunctions(self):
        print "1 ========================================================"

        functest1 = self.kspread.function("SCRIPT_TEST1")
        functest1.minparam = 1
        functest1.maxparam = 1
        functest1.comment = (
            "The SCRIPT_TEST1() function demonstrates how to use scripting"
            "functions. All it does is to take a string as argument and"
            "return the same string."
        )
        functest1.syntax = "SCRIPT_TEST1(string)"
        functest1.addParameter("String", "The string that should be returned")
        functest1.addExample("SCRIPT_TEST1(\"Some string\")")
        def functest1callback(*args):
            return "%s" % args
        functest1.connect("called(QVariantList)", functest1callback)
        functest1.registerFunction()

        #functest1 = self.kspread.function("SCRIPT_TEST2")
        #functest1.minparam = 1
        #functest1.maxparam = 1
        #functest1.comment = (
            #"The SCRIPT_TEST2() function demonstrates how to use scripting"
            #"functions. All it does is to take an integer as argument and"
            #"return the same integer."
        #)
        #functest1.syntax = "SCRIPT_TEST2(integer)"
        #functest1.addParameter("Int", "The integer that should be returned")
        #functest1.addExample("SCRIPT_TEST2(12345)")

        #functest1 = self.kspread.function("SCRIPT_TEST1")
        #functest1.minparam = 2
        #functest1.maxparam = -1
        #functest1.comment = (
            #"The SCRIPT() function calls a function in an external "
            #"scriptfile. The script file references a file relative "
            #"to your KSpread scripts folder while the function name "
            #"should be an existing function within that script file."
        #)
        #functest1.syntax = "SCRIPT(scriptfile;functionname;...)"
        #functest1.addParameter("String", "The name of the script file")
        #functest1.addParameter("String", "The name of the function")
        #functest1.addExample("SCRIPT(\"myscript1.py\";\"myfunction1\")")
        #functest1.addExample("SCRIPT(\"myscript2.rb\";\"myfunction2\";\"optional argument\")")
        #functest1.addExample("SCRIPT(\"myscript3.js\";\"myfunction3\";17;20.0;\"string\")")
        #functest1.addExample("SCRIPT(\"subfolder/myscript4.py\";\"myfunction4\"")

        #a = functest1.action()
        #a.setInterpreter("python")
        #a.setCode("lambda x: x**2")

        #def functiontest1(*args):
        #   return "TeStReSuLt"
        #functest1.__connect__("SIGNAL(...)", self.scriptaction, functiontest1)

        #functest1.setAction(self.scriptaction)
        #functest1.setAction(self.scriptaction, "functionname")

        #functest2 = self.kspread.function("SCRIPT_TEST2")
        print "2 ========================================================"

Setup( self )
