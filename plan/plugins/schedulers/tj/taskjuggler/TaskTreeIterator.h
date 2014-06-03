/*
 * TaskTreeIterator.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _TaskTreeIterator_h_
#define _TaskTreeIterator_h_

#include "CoreAttributesTreeIterator.h"

class TaskTreeIterator : public virtual CoreAttributesTreeIterator
{
public:
    explicit TaskTreeIterator(Task* r, IterationMode m = leavesOnly)
        : CoreAttributesTreeIterator(r, m) { }
    virtual ~TaskTreeIterator() { }

    Task* operator*() { return static_cast<Task*>(current); }
    Task* operator++()
    {
        return static_cast<Task*>(CoreAttributesTreeIterator::operator++());
    }
} ;

#endif

