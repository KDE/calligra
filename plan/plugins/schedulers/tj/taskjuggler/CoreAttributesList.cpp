/*
 * CoreAttributesList.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 * Copyright (c) 2011 by Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "CoreAttributesList.h"

namespace TJ
{


CoreAttributesList::~CoreAttributesList()
{
    if (autoDelete()) {
        setAutoDelete(false);
        while (!isEmpty()) {
            delete takeFirst();
        }
        setAutoDelete(true);
    }
//     if (autoDelete())
//     {
//         /* We need to make sure that the CoreAttributes are first removed from
//          * the list and then deleted. */
//         setAutoDelete(false);
//         while (!isEmpty())
//         {
//             CoreAttributes* tp = getFirst();
//             removeRef(tp);
//             delete tp;
//         }
//         setAutoDelete(true);
//     }
}

void
CoreAttributesList::setAutoDelete(bool on)
{
    m_autodelete = on;
}

bool
CoreAttributesList::autoDelete() const
{
    return m_autodelete;
}

void
CoreAttributesList::deleteContents()
{
    // Don't understand this code (danders)
    while (!isEmpty()) {
        for (CoreAttributesListIterator li(*this); *li; ++li)
            if ((*li)->getParent() == 0)
            {
                delete *li;
                break;
            }
    }
}

void
CoreAttributesList::setSorting(int s, int level)
{
    if (level >=0 && level < maxSortingLevel)
        sorting[level] = s;
    else
        qFatal("CoreAttributesList::setSorting: level out of range: %d",
               level);
}

void
CoreAttributesList::sort()
{
    QList<CoreAttributes*> lst = *this;
    clear();
    QStringList s; for(int i = 0; i < lst.count(); ++i) s << lst.at(i)->getId();
    qDebug()<<"CoreAttributesList::sort:"<<s;
    while (!lst.isEmpty()) {
        inSort(lst.takeLast());
    }
    s.clear(); for(int i = 0; i < lst.count(); ++i) s << lst.at(i)->getId();
    qDebug()<<"CoreAttributesList::sort: sorted"<<s;
}

void
CoreAttributesList::createIndex(bool initial)
{
    /* In "initial" mode the sequenceNo is set. This should only be done once
     * for each list. In the other mode the index is set. This is most likely
     * called after the sorting criteria have been changed. */
    int i = 1;
    if (initial)
    {
        uint hNo = 1;
        for (int pos = 0; pos < count(); ++pos)
        {
            CoreAttributes *a = at(pos);
            a->setSequenceNo(i);
            if (a->getParent() == 0)
                a->setHierarchNo(hNo++);
        }
    }
    else
    {
        sort();
        for (int pos = 0; pos < count(); ++pos)
        {
            CoreAttributes *a = at(pos);
            a->setIndex(i);
            // Reset all hierarchIndices to 0.
            a->setHierarchIndex(0);
        }
        // Then number them again.
        uint hNo = 1;
        for (int pos = 0; pos < count(); ++pos)
        {
            CoreAttributes *a = at(pos);
            a->setHierarchIndex(hNo);
            if (a->getParent() == 0)
                hNo++;
        }
    }
}

int
CoreAttributesList::getIndex(const QString& id) const
{
    for (int pos = 0; pos < count(); ++pos) {
        CoreAttributes *a = at(pos);
        if (a->getId() == id)
            return a->getIndex();
    }
    return -1;
}

uint
CoreAttributesList::maxDepth() const
{
    uint md = 0;
    for (int pos = 0; pos < count(); ++pos) {
        CoreAttributes *a = at(pos);
        if (a->treeLevel() + 1 > md)
            md = a->treeLevel() + 1;
    }
    return md;
}

bool
CoreAttributesList::isSupportedSortingCriteria(int sc)
{
    switch (sc)
    {
    case SequenceUp:
    case SequenceDown:
    case TreeMode:
    case FullNameDown:
    case FullNameUp:
    case IndexUp:
    case IndexDown:
    case IdUp:
    case NameUp:
    case NameDown:
        return true;
    default:
        return false;
    }
}

int CoreAttributesList::inSort(CoreAttributes* attr)
{
    int i = 0;
    for (; i < count(); ++i) {
        int r = compareItems(attr, at(i));
        if (r < 0) {
            break;
        }
    }
    insert(i, attr);
    return i;
}
                                          
int
CoreAttributesList::compareItemsLevel(CoreAttributes* c1, CoreAttributes* c2,
                                      int level)
{
    if (level < 0 || level >= maxSortingLevel)
        return -1;

    switch (sorting[level])
    {
    case SequenceUp:
        return c1->getSequenceNo() == c2->getSequenceNo() ? 0 :
            c1->getSequenceNo() < c2->getSequenceNo() ? -1 : 1;
    case SequenceDown:
        return c1->getSequenceNo() == c2->getSequenceNo() ? 0 :
            c1->getSequenceNo() > c2->getSequenceNo() ? -1 : 1;
    case TreeMode:
    {
        if (level == 0)
            return compareTreeItemsT(this, c1, c2);
        else
            return c1->getSequenceNo() < c2->getSequenceNo() ? -1 : 1;
    }
    case FullNameDown:
    {
        QString fn1;
        c1->getFullName(fn1);
        QString fn2;
        c2->getFullName(fn2);
        return fn1.compare(fn2);
    }
    case FullNameUp:
    {
        QString fn1;
        c1->getFullName(fn1);
        QString fn2;
        c2->getFullName(fn2);
        return fn2.compare(fn1);
    }
    case IndexUp:
        return c2->getIndex() == c1->getIndex() ? 0 :
            c2->getIndex() < c1->getIndex() ? -1 : 1;
    case IndexDown:
        return c1->getIndex() == c2->getIndex() ? 0 :
            c1->getIndex() > c2->getIndex() ? -1 : 1;
    case IdUp:
        return QString::compare(c1->getId(), c2->getId());
    case IdDown:
        return QString::compare(c2->getId(), c1->getId());
    case NameUp:
        return c1->getName().compare(c2->getName());
    case NameDown:
        return c2->getName().compare(c1->getName());
    default:
        qFatal("CoreAttributesList:compareItemsLevel: "
               "Please implement sorting for mode (%d/%d) in sub class!",
               sorting[level], level);
    }
    return 0;
}

int
CoreAttributesList::compareItems(CoreAttributes* c1, CoreAttributes* c2)
{
    int res;
    for (int i = 0; i < CoreAttributesList::maxSortingLevel; ++i)
        if ((res = compareItemsLevel(c1, c2, i)) != 0)
            return res;
    return res;
}

//static
QStringList CoreAttributesList::getSortCriteria()
{
   QStringList lst;
   lst << "SequenceUp" << "SequenceDown"
       << "TreeMode" << "NameUp" << "NameDown" << "FullNameUp"
       << "FullNameDown" << "IdUp" << "IdDown" << "IndexUp" << "IndexDown"
       << "StatusUp" << "StatusDown" << "CompletedUp" << "CompletedDown"
       << "PrioUp" << "PrioDown"
       << "ResponsibleUp" << "ResponsibleDown"
       << "MinEffortUp" << "MinEffortDown"
       << "MaxEffortUp" << "MaxEffortDown"
       << "RateUp" << "RateDown"
       << "StartUp" << "StartDown" << "EndUp" << "EndDown"
       << "CriticalnessUp" << "CriticalnessDown"
       << "PathCriticalnessUp" << "PathCriticalnessDown"
        ;
    return lst;
}

} // namespace TJ

QDebug operator<<( QDebug dbg, const TJ::CoreAttributesList& lst )
{
    QStringList s;
    for ( int i = 0; i < TJ::CoreAttributesList::maxSortingLevel; ++i ) {
        s << TJ::CoreAttributesList::getSortCriteria().at( lst.getSorting( i ) );
    }
    dbg.nospace() << "CoreAttributeList{sort: " << s.join("|") << " (";
    for( int i = 0; i < lst.count(); ++i ) {
        dbg << lst.at( i );
        if ( i  < lst.count() - 1 ) {
            dbg.nospace() << ',';
        }
    }
    dbg.nospace() << ")}";
    return dbg;
}
