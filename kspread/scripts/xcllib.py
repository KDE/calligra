from math import *

print "Library started"

def TestMe():
	return 100.2

def xclCellTest( table, column, row):
	print "xclCell"
	return 100

def Sum( rect ):
	v = 0.0
	table = rect[ 0 ]
	left = rect[ 1 ]
	top = rect[ 2 ]
	right = rect[ 3 ]
	bottom = rect[ 4 ]
	for x in range( left, right + 1 ):
		for y in range( top, bottom + 1 ):
			v = v + Cell( table, x, y )
	return v

def Pyt( a,b ):
	return a*b

