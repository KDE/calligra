#!/usr/bin/python

from string import *
from sys import *

def filter( text ):
    lines = split( text, "\n" );
    
    result = ""
    result = result + '<?xml version="1.0"?>\n'
    result = result + '<DOC author="Torben Weis" email="weis@kde.org" editor="KSpread" mime="application/x-kspread" >\n'
    result = result + ' <MAP>\n'
    result = result + '  <TABLE name="Tabelle 1">\n'

    row = 1
    
    for line in lines :
        column = 1
        cells = split( line, "," );
        for cell in cells:
	    if cell[-1:] == "\n":
	       cell = cell[:-1]
	    result = result + '   <CELL row="%i" column="%i">%s</CELL>\n' % ( row, column, cell )
	    column = column + 1
        row = row + 1

    result = result + '  </TABLE>\n'
    result = result + ' </MAP>\n'
    result = result + '</DOC>\n'
    
    return result
