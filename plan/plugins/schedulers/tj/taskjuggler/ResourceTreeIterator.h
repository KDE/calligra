/*
 * ResourceTreeIterator.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _ResourceTreeIterator_h_
#define _ResourceTreeIterator_h_

#include "CoreAttributesTreeIterator.h"

namespace TJ
{

template <class T, class R>
class ResourceTreeIteratorT : public virtual T
{
public:
    ResourceTreeIteratorT(R* r, IterationMode m = leavesOnly)
        : T(r, m) { }
    virtual ~ResourceTreeIteratorT() { }

    R* operator*() { return static_cast<R*>(T::current); }
    R* operator++()
    {
        return static_cast<R*>(T::operator++());
    }
} ;

typedef ResourceTreeIteratorT<CoreAttributesTreeIterator, Resource>
    ResourceTreeIterator;
typedef ResourceTreeIteratorT<ConstCoreAttributesTreeIterator, const Resource>
    ConstResourceTreeIterator;


} // namespace TJ

#endif
