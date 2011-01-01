#!/usr/bin/env kross

import time, Kross, KSpread

func = KSpread.function("PYTIME")
func.minparam = 0
func.maxparam = 1
func.comment = "The PYTIME() function displays the current datetime."
func.syntax = "PYTIME(string)"
func.addParameter("String", "The datetime format string.")
func.addExample("PYTIME()")
func.addExample("PYTIME(\"%H:%M.%S\")")
func.addExample("PYTIME(\"%Y-%M-%d\")")

def update(args):
    try:
        func.result = time.strftime(args[0] or "%H:%M.%S")
    except:
        func.error = "Invalid format"

func.connect("called(QVariantList)", update)
func.registerFunction()
