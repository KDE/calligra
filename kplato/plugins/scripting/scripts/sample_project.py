#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, sys, traceback, tempfile, zipfile
import Kross
import KPlato

def printNodes( node, props, types = None ):
    printNode( node, props, types )
    for i in range( node.childCount() ):
        printNodes( KPlato.childAt( node, i ), props, types )

def printNode( node, props, types = None ):
    if types is None or node.type() in types:
        for prop in props:
            print "%-25s" % ( node.data( prop ) ),
        print

def printGroup( group, props ):
    for prop in props:
        print "%-25s" % ( group.data( prop ) ),
    print
    for i in range( group.resourceCount() ):
        printResource( KPlato.resourceAt( group, i ), props )

def printResource( resource, props ):
    for prop in props:
        print "%-25s" % ( resource.data( prop ) ),
    print

def printSchedules():
    print "%-10s %-25s" % ( "Identity", "Name" )
    for i in range( KPlato.scheduleCount() ):
        printSchedule( KPlato.scheduleAt( i ) )
        
    print


def printSchedule( sch ):
    print "%-10s %-25s" % ( sch.id(), sch.name() )
    for i in range( sch.childCount() ):
        printSchedule( KPlato.scheduleAt( sch, i ) )


#------------------------
proj = KPlato.project()

nodeprops = ['NodeWBSCode', 'NodeName', 'NodeType', 'NodeResponsible' ]
print "Print tasks and milestones in arbitrary order:"
# print the localized headers
for prop in nodeprops:
    print "%-25s" % (proj.nodeHeaderData( prop ) ),
print
printNodes( proj, nodeprops, [ 'Task', 'Milestone' ] )
print

print "Print all nodes including project:"
for prop in nodeprops:
    print "%-25s" % (proj.nodeHeaderData( prop ) ),
print
printNodes( proj, nodeprops )
print

print "Print Resources:"
resprops = [ 'ResourceName', 'ResourceType', 'ResourceEmail', 'ResourceCalendar' ]
# print the localized headers
for prop in resprops:
    print "%-25s" % (proj.resourceHeaderData( prop ) ),
print
for index in range( KPlato.resourceGroupCount() ):
    g = KPlato.resourceGroupAt( index )
    printGroup( g, resprops )

print

print "Print Schedules:"
printSchedules()
print

#for i in range( KPlato.nodeCount() ):
    #print KPlato.nodeAt( i ).plannedEffortCostPrDay( "2007-09-12", "2007-11-10", "" )
print proj.plannedEffortCostPrDay( "2007-09-12", "2007-11-10", "" )