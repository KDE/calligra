/*
 * CoreAttributesTreeIterator.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _CoreAttributesTreeIterator_h_
#define _CoreAttributesTreeIterator_h_

namespace TJ
{

enum IterationMode { leavesOnly = 0, parentAfterLeaves };

class CoreAttributes;

template <class T>
class CoreAttributesTreeIteratorT
{
public:

    CoreAttributesTreeIteratorT(T* root, IterationMode m = leavesOnly);
    ~CoreAttributesTreeIteratorT() { }

    T* operator*() { return current; }
    T* operator++();

protected:
    T* current;
private:
    IterationMode iMode;
    T* root;
} ;

template <class T>
CoreAttributesTreeIteratorT<T>::CoreAttributesTreeIteratorT(T* r,
                                                            IterationMode m) :
    current(r),
    iMode(m),
    root(r)
{
    while (current->hasSubs())
        current = current->getSubList().first();
}

template <class T>
T*
CoreAttributesTreeIteratorT<T>::operator++()
{
    if (current == 0)
        return 0;

    while (current != root)
    {
        // Find the current CA in the parent's sub list.
        CoreAttributesListIterator
            cli(current->getParent()->getSubListIterator());
        for ( ; cli.hasNext() && cli.peekNext() != current; cli.next())
            ;
        // Check if there is another task in the sub list.
        if (cli.hasNext())
        {
            // Find the first leaf in this sub list.
            current = cli.next();
            while (current->hasSubs())
                current = current->getSubList().first();
            // This is the new current task.
            return current;
        }
        // End of sub list reached. Try parent node then.
        current = current->getParent();
        if (iMode == parentAfterLeaves)
            return current;
    }
    return (current = 0);
}

typedef CoreAttributesTreeIteratorT<CoreAttributes> CoreAttributesTreeIterator;
typedef CoreAttributesTreeIteratorT<const CoreAttributes>
    ConstCoreAttributesTreeIterator;

} // namespace TJ

#endif
