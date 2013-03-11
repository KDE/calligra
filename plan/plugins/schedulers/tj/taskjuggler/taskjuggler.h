/*
 * taskjuggler.h - TaskJuggler
 *
 * Copyright (c) 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 * Copyright (c) 2011 by Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _taskjuggler_h_
#define _taskjuggler_h_

#define TJURL "http://www.taskjuggler.org"

typedef enum CAType
{
    CA_Undefined = 0, CA_Task, CA_Resource, CA_Account, CA_Shift, CA_Scenario
} CAType;

typedef enum CustomAttributeType
{
    CAT_Undefined = 0, CAT_Reference, CAT_Text
} CustomAttributeType;

typedef enum TaskStatus
{
    Undefined = 0, NotStarted, InProgressLate, InProgress, OnTime,
    InProgressEarly, Finished, Late
} TaskStatus;

typedef enum AccountType { AllAccounts = 0, Cost, Revenue } AccountType;

typedef enum LoadUnit
{
    minutes = 0, hours, days, weeks, months, years, shortAuto, longAuto
} LoadUnit;

// type of log message
namespace TJ {
    enum LogMessageType { DebugMsg = 0, InfoMsg, WarningMsg, ErrorMsg };
}

#endif

