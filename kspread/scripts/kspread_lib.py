from KSpread import *
import CORBA

bookIOR = ""

def book():
    return Book( ior = bookIOR )

def table():
    return Table( ior = tableIOR )

def doc():
    return Document( ior = docIOR )
