#!/usr/bin/env kross

import time, Kross, KSpread

T = Kross.module("kdetranslation")

func = KSpread.function("PYTIME")
func.minparam = 0
func.maxparam = 1
func.comment = T.i18n("The PYTIME() function displays the current datetime.")
func.syntax = T.i18n("PYTIME(string)")
func.addParameter("String", T.i18n("The datetime format string."))
func.addExample(T.i18n("PYTIME()"))
func.addExample(T.i18n("PYTIME(\"%H:%M.%S\")"))
func.addExample(T.i18n("PYTIME(\"%Y-%M-%d\")"))

def update(args):
    try:
        func.result = time.strftime(args[0] or "%H:%M.%S")
    except:
        func.error = T.i18n("Invalid format")

func.connect("called(QVariantList)", update)
func.registerFunction()
