#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, sys, traceback, tempfile, zipfile
import Kross
import Plan

State_Started = 0
State_StartedLate = 1
State_StartedEarly = 2
State_Finished = 3
State_FinishedLate = 4
State_FinishedEarly = 5
State_Running = 6
State_RunningLate = 7
State_RunningEarly = 8
State_ReadyToStart = 9        # all precceeding tasks finished (if any)
State_NotReadyToStart = 10    # all precceeding tasks not finished (must be one or more)
State_NotScheduled = 11

def testBit(int_type, offset):
    mask = 1 << offset
    return ( int_type & mask ) > 0

def state( int_type ):
    if testBit( int_type, State_NotScheduled ) is True:
        return "Not scheduled"
    if testBit( int_type, State_FinishedEarly ) is True:
        return "Finished early"
    if testBit( int_type, State_FinishedLate ) is True:
        return "Finished late"
    if testBit( int_type, State_Finished ) is True:
        return "Finished"
    if testBit( int_type, State_ReadyToStart ) is True:
        return "Ready to start"
    if testBit( int_type, State_NotReadyToStart ) is True:
        return "Not ready to start"
    if testBit( int_type, State_StartedLate ) is True:
        return "Started late"
    if testBit( int_type, State_StartedEarly ) is True:
        return "Started early"
    if testBit( int_type, State_RunningEarly ) is True:
        return "Running early"
    if testBit( int_type, State_RunningLate ) is True:
        return "Running late"
    if testBit( int_type, State_Running ) is True:
        return "Running"
    if int_type > 0:
        return "Error: Invalid state %d" % ( int_type )
    return "None"

def printStates( node, schedule ):
    printState( node, schedule )
    for i in range( node.childCount() ):
        printStates( node.childAt( i ), schedule )

def printState( node, schedule ):
    if node.type() in [ 'Task' ]:
        st = Plan.project().data( node, 'Status', 'EditRole', schedule )
        print "%-30s %-20s %20s" % ( 
            Plan.project().data( node, 'Name', 'DisplayRole', schedule ),
            Plan.project().data( node, 'Status', 'DisplayRole', schedule ),
            state( int( st ) ) )

def printNodes( node, props, schedule, types = None ):
    printNode( node, props, schedule, types )
    for i in range( node.childCount() ):
        printNodes( node.childAt( i ), props, schedule, types )

def printNode( node, props, schedule, types = None ):
    if types is None or node.type() in types:
        for prop in props:
            print "%-25s" % ( Plan.project().data( node, prop[0], prop[1], schedule ) ),
        print

def printGroup( group, props ):
    for prop in props:
        print "%-25s" % ( Plan.project().data( group, prop ) ),
    print
    for i in range( group.resourceCount() ):
        printResource( group.resourceAt( i ), props )

def printResource( resource, props ):
    for prop in props:
        print "%-25s" % ( Plan.project().data( resource, prop ) ),
    print

def printSchedules():
    print "%-10s %-25s %-10s" % ( "Identity", "Name", "Scheduled" )
    for i in range( proj.scheduleCount() ):
        printSchedule( proj.scheduleAt( i ) )
    print


def printSchedule( sch ):
    print "%-10s %-25s %-10s" % ( sch.id(), sch.name(), sch.isScheduled() )
    for i in range( sch.childCount() ):
        printSchedule( sch.childAt( i ) )

def printEffortCost( name, values ):
    print "%-20s" % ( name )
    for d, v in sorted( values.iteritems() ):
        e = v[0]
        c = v[1]
        print "%-20s %-15s %20f %20f" % ( "", d, e, c )

def printProjectBusyinfo( proj ):
    print "%-20s %-30s %-30s %8s" % ( "Resource", "Start", "End", "Load" )
    id = proj.scheduleAt( 0 ).id()
    for gi in range( proj.resourceGroupCount() ):
        g = proj.resourceGroupAt( gi )
        for ri in range( g.resourceCount() ):
            r = g.resourceAt( ri )
            printBusyinfo( r, r.appointmentIntervals( id ) )
            print
        
    print

def printBusyinfo( res, lst ):
    name = Plan.project().data( res, 'Name' )
    for interval in lst:
        print "%-20s %-30s %-30s %8s" % ( name, interval[0], interval[1], interval[2] )
        name = ""

def printProjectCalendars( proj ):
    for c in range( proj.calendarCount() ):
        printChildCalendars( proj.calendarAt ( c ) )

def printChildCalendars( calendar ):
    print calendar.name()
    for c in range( calendar.childCount() ):
        printChildCalendars( calendar.childAt ( c ) )


def printExternalProjects( proj ):
    projects = proj.externalProjects()
    if len(projects) == 0:
        print "No external project appointments"
        return
    if len(projects) % 2 == 1:
        print "Illegal id/name pair in list: %s" % projects
        return

    print "%-35s %s" % ( "Identity", "Name" )
    for c in projects:
        print "%-35s %s" % ( c[0], c[1] )

def printTaskEffortCost( parent ):
    for i in range( parent.childCount() ):
        node = parent.childAt( i )
        name = Plan.project().data( node, 'Name' )
        printEffortCost( name, node.plannedEffortCostPrDay( "2007-09-12", "2007-09-18", sid ) )

#------------------------
proj = Plan.project()

sid = -1;
# get a schedule id
if proj.scheduleCount() > 0:
    sid = proj.scheduleAt( 0 ).id()

print "Using schedule id: %-3s" % ( sid )
print

nodeprops = [['WBSCode', 'DisplayRole'], ['Name', 'DisplayRole'], ['Type', 'DisplayRole'], ['Responsible', 'DisplayRole'], ['Status', 'EditRole'] ]
print "Print tasks and milestones in arbitrary order:"
# print the localized headers
for prop in nodeprops:
    print "%-25s" % (proj.taskHeaderData( prop ) ),
print
printNodes( proj, nodeprops, sid, [ 'Task', 'Milestone' ] )
print

print "Print all nodes including project:"
for prop in nodeprops:
    print "%-25s" % (proj.taskHeaderData( prop ) ),
print
printNodes( proj, nodeprops, sid )
print

print "Print Resources:"
resprops = [ 'Name', 'Type', 'Email', 'Calendar' ]
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


print "Print Effort/Cost for each task:"
print "%-20s %-10s %-10s %-10s" % ( 'Name', 'Date', 'Effort', 'Cost' )
printTaskEffortCost( proj )
print
print "Print Effort/Cost for the project:"
print "%-20s %-10s %-10s %-10s" % ( 'Name', 'Date', 'Effort', 'Cost' )
printEffortCost( proj.name(), proj.plannedEffortCostPrDay( "2007-09-12", "2007-09-17", sid ) )
print
print "Print Busy information for all resources in the project:"
printProjectBusyinfo( proj )
print

print "Print the calendars in the project:"
printProjectCalendars( proj )
print

print "Print planned Effort/Cost for each account:"
print "%-20s %-15s %20s %20s" % ( 'Name', 'Date', 'Effort', 'Cost' )
for i in range( proj.accountCount() ):
    account = proj.accountAt( i )
    name = Plan.project().data( account, 'Name' )
    printEffortCost( name, account.plannedEffortCostPrDay( sid ) )
print

print "Print actual Effort/Cost for each account:"
print "%-20s %-15s %20s %20s" % ( 'Name', 'Date', 'Effort', 'Cost' )
for i in range( proj.accountCount() ):
    account = proj.accountAt( i )
    name = Plan.project().data( account, 'Name' )
    printEffortCost( name, account.actualEffortCostPrDay( sid ) )
print

print "Print BCWS for the project:"
print "%-20s %-15s %20s %20s" % ( 'Name', 'Date', 'Effort', 'Cost' )
printEffortCost( proj.name(), proj.bcwsPrDay( sid ) )
print
print "Print BCWP for the project:"
print "%-20s %-15s %20s %20s" % ( 'Name', 'Date', 'Effort', 'Cost' )
printEffortCost( proj.name(), proj.bcwpPrDay( sid ) )
print

print "Print ACWP for the project:"
print "%-20s %-15s %20s %20s" % ( 'Name', 'Date', 'Effort', 'Cost' )
printEffortCost( proj.name(), proj.acwpPrDay( sid ) )
print

print "Print Task status:"
printStates( proj, sid )
print

print "Print external projects:"
printExternalProjects( proj )
print
