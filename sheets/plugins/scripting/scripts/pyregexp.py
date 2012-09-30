#!/usr/bin/env kross

import re, Kross, KSpread

T = Kross.module("kdetranslation")

func = KSpread.function("PYREGEXP")
func.minparam = 3
func.maxparam = 3
func.comment = T.i18n("The PYREGEXP() function replaces substring in the input string with a given string using regular expression.")
func.syntax = T.i18n("PYREGEXP(string)")
func.addParameter("String", T.i18n("The input string."))
func.addParameter("String", T.i18n("The regular expression."))
func.addParameter("String", T.i18n("Replace with."))
func.addExample(T.i18n("PYREGEXP(\"Some String\",\"(S|m)\",\"A\")"))

def update(args):
    s = args[0]
    regexp = args[1]
    repl = args[2]
    try:
        p = re.compile(regexp)
        func.result = p.sub(repl, s)
    except:
        func.error = T.i18n("Invalid regexp")

func.connect("called(QVariantList)", update)
func.registerFunction()
