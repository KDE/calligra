#!/usr/bin/env kross

import re, Kross, KSpread

func = KSpread.function("PYREGEXP")
func.minparam = 3
func.maxparam = 3
func.comment = "The PYREGEXP() function displays the current datetime."
func.syntax = "PYREGEXP(string)"
func.addParameter("String", "The input string.")
func.addParameter("String", "The regular expression.")
func.addParameter("String", "Replace with.")
func.addExample("PYREGEXP(\"Some String\",\"(S|m)\",\"A\")")

def update(args):
    s = args[0]
    regexp = args[1]
    repl = args[2]
    try:
        p = re.compile(regexp)
        func.result = p.sub(repl, s)
    except:
        func.error = "Invalid regexp"

func.connect("called(QVariantList)", update)
func.registerFunction()
