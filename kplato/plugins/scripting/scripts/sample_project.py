#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, sys, traceback, tempfile, zipfile
import Kross
import KPlato

def printNodes( node, props, types = None ):
    printNode( node, props, types )
    for i in range( node.childCount() ):
        printNodes( node.childAt( i ), props, types )

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
        printResource( group.resourceAt( i ), props )

def printResource( resource, props ):
    for prop in props:
        print "%-25s" % ( resource.data( prop ) ),
    print

def printSchedules():
    print "%-10s %-25s" % ( "Identity", "Name" )
    for i in range( proj.scheduleCount() ):
        printSchedule( proj.scheduleAt( i ) )
        
    print


def printSchedule( sch ):
    print "%-10s %-25s" % ( sch.id(), sch.name() )
    for i in range( sch.childCount() ):
        printSchedule( sch.childAt( i ) )

def printEffortCost( name, values ):
    print "%-20s" % ( name )
    for d, v in sorted( values.iteritems() ):
        e = v[0]
        c = v[1]
        print "%-20s %-10s %-10f %-10f" % ( "", d, e, c )

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
for index in range( proj.resourceGroupCount() ):
    g = proj.resourceGroupAt( index )
    printGroup( g, resprops )

print

print "Print Schedules:"
printSchedules()
print


sid = -1;
# get a schedule id
if proj.scheduleCount() > 0:
    sid = proj.scheduleAt( 0 ).id()

print "Print Effort/Cost for each node:"
print "%-20s %-10s %-10s %-10s" % ( 'Name', 'Date', 'Effort', 'Cost' )
for i in range( proj.nodeCount() ):
    node = proj.nodeAt( i )
    name = node.data( 'NodeName' )
    printEffortCost( name, node.plannedEffortCostPrDay( "2007-09-12", "2007-09-18", sid ) )

print "Print Effort/Cost for the project:"

print "%-20s %-10s %-10s %-10s" % ( 'Name', 'Date', 'Effort', 'Cost' )
name = proj.data( 'NodeName' )
printEffortCost( name, proj.plannedEffortCostPrDay( "2007-09-12", "2007-09-17", sid ) )

print
