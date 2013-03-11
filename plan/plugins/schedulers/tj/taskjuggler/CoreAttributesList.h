/*
 * CoreAttributesList.h - TaskJuggler
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
#ifndef _CoreAttributesList_h_
#define _CoreAttributesList_h_

#include "kplatotj_export.h"

#include <QList>
#include <KDebug>

#include "CoreAttributes.h"

class QString;

namespace TJ
{

/**
 * @short The class stores a list of CoreAttributes.
 * @see CoreAttributes
 * @author Chris Schlaeger <cs@kde.org>
 */
class KPLATOTJ_EXPORT CoreAttributesList : public QList<CoreAttributes*>
{
public:
    CoreAttributesList() : m_autodelete(false)
    {
        for (int i = 0; i < maxSortingLevel; i++)
            sorting[i] = SequenceUp;
    }
    CoreAttributesList(const CoreAttributesList& l) :
        QList<CoreAttributes*>(l),
        m_autodelete(false)
    {
        for (int i = 0; i < maxSortingLevel; i++)
            sorting[i] = l.sorting[i];
    }

    virtual ~CoreAttributesList();

    void setAutoDelete(bool on);
    bool autoDelete() const;

    void deleteContents();

    enum SortCriteria {
        SequenceUp = 0, SequenceDown,
        TreeMode, NameUp, NameDown, FullNameUp,
        FullNameDown, IdUp, IdDown, IndexUp, IndexDown,
        StatusUp, StatusDown, CompletedUp, CompletedDown,
        PrioUp, PrioDown,
        ResponsibleUp, ResponsibleDown,
        MinEffortUp, MinEffortDown,
        MaxEffortUp, MaxEffortDown,
        RateUp, RateDown,
        StartUp, StartDown, EndUp, EndDown,
        CriticalnessUp, CriticalnessDown,
        PathCriticalnessUp, PathCriticalnessDown
    };

    static const int maxSortingLevel = 3;
    void setSorting(int s, int level);
    int getSorting(int level) const { return level < maxSortingLevel ? sorting[ level ] : 0; }
    void sort();
    void createIndex(bool initial = false);
    int getIndex(const QString& id) const;
    uint maxDepth() const;

    static bool isSupportedSortingCriteria(int sc);

    virtual int compareItemsLevel(CoreAttributes* c1, CoreAttributes* c2,
                                  int level);

    int inSort(CoreAttributes *attr);

    static QStringList getSortCriteria();

protected:
    virtual int compareItems(CoreAttributes* c1, CoreAttributes* c2);

    bool m_autodelete;
    int sorting[maxSortingLevel];
} ;

/**
 * @short Iterator for CoreAttributesList objects.
 * @author Chris Schlaeger <cs@kde.org>
 */
class CoreAttributesListIterator : public QListIterator<CoreAttributes*>
{
public:
    CoreAttributesListIterator(const CoreAttributesList& l) :
        QListIterator<CoreAttributes*>(l) { }
    virtual ~CoreAttributesListIterator() { }
    void operator++() { if (hasNext()) next(); }
    CoreAttributes *operator*() { return hasNext() ? peekNext() : 0; }
} ;

template<class TL, class T> int compareTreeItemsT(TL* list, T* c1, T* c2)
{
    if (c1 == c2)
        return 0;

    QList<T*> cl1, cl2;
    int res1 = 0;
    for ( ; c1 || c2; )
    {
        if (c1)
        {
            cl1.prepend(c1);
            c1 = c1->getParent();
        }
        else
            res1 = -1;
        if (c2)
        {
            cl2.prepend(c2);
            c2 = c2->getParent();
        }
        else
            res1 = 1;
    }

    QListIterator<T*> cal1(cl1);
    QListIterator<T*> cal2(cl2);
    while (cal1.hasNext() && cal2.hasNext())
    {
        CoreAttributes *a1 = cal1.next();
        CoreAttributes *a2 = cal2.next();
        int res;
        for (int j = 1; j < CoreAttributesList::maxSortingLevel; ++j)
        {
            if ((res = list->compareItemsLevel(a1, a2, j)) != 0)
                return res;
        }
        if ((res = a1->getSequenceNo() - a2->getSequenceNo()) != 0)
            return res < 0 ? -1 : 1;
    }
    return res1;
}

} // namespace TJ

KPLATOTJ_EXPORT QDebug operator<<( QDebug dbg, const TJ::CoreAttributesList& lst );

#endif
