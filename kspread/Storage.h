/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_STORAGE
#define KSPREAD_STORAGE

#include <QTimer>

#include <koffice_export.h>

#include "Condition.h"
#include "Region.h"
#include "RTree.h"
#include "Validity.h"

static const int g_garbageCollectionTimeOut = 60000; // one minute

namespace KSpread
{

class Sheet;

/**
 * A custom spatial storage based on an R-Tree.
 */
template<typename T>
class KSPREAD_EXPORT Storage
{
public:
    Storage( Sheet* sheet );
    virtual ~Storage();

    /**
     * \return the stored value at the position \p point .
     */
    T at(const QPoint& point) const;

    QList< QPair<QRectF, T> > undoData(const QRect& rect) const;

    /**
     * Assigns \p data to \p region .
     */
    void insert(const Region& region, const T& data);

protected:
    virtual void garbageCollectionInitialization();
    virtual void garbageCollection();

    QTimer*     m_garbageCollectionInitializationTimer;
    QTimer*     m_garbageCollectionTimer;

private:
    Sheet*      m_sheet;
    RTree<T>    m_tree;
    QList< QPair<QRectF,T> > m_possibleGarbage;
    QList<T>    m_storedData;
};

template<typename T>
Storage<T>::Storage( Sheet* sheet )
    : m_sheet( sheet )
{
    m_garbageCollectionInitializationTimer = new QTimer();
    m_garbageCollectionInitializationTimer->start(g_garbageCollectionTimeOut);
    m_garbageCollectionTimer = new QTimer();
    m_garbageCollectionTimer->start();
}

template<typename T>
Storage<T>::~Storage()
{
    delete m_garbageCollectionInitializationTimer;
    delete m_garbageCollectionTimer;
}

template<typename T>
T Storage<T>::at(const QPoint& point) const
{
    QList<T> results = m_tree.contains(point);
    return results.isEmpty() ? T() : results.last();
}

template<typename T>
QList< QPair<QRectF,T> > Storage<T>::undoData(const QRect& rect) const
{
    QList< QPair<QRectF,T> > result = m_tree.intersectingPairs(rect);
    for ( int i = 0; i < result.count(); ++i )
    {
        // trim the rects
        result[i].first = result[i].first.intersected( rect );
    }
    return result;
}

template<typename T>
void Storage<T>::insert(const Region& region, const T& _data)
{
    T data;
    // lookup already used data
    if ( m_storedData.contains(_data) )
        data = m_storedData[ m_storedData.indexOf( _data ) ];
    else
    {
        data = _data;
        m_storedData.append( _data );
    }

    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
    {
        m_tree.insert((*it)->rect(), data);
    }
}

template<typename T>
void Storage<T>::garbageCollectionInitialization()
{
    // last garbage collection finished?
    if ( !m_possibleGarbage.isEmpty() )
        return;
    m_possibleGarbage = m_tree.intersectingPairs( QRect( 1, 1, KS_colMax, KS_rowMax ) );
}

template<typename T>
void Storage<T>::garbageCollection()
{
    // any possible garbage left?
    if ( m_possibleGarbage.isEmpty() )
        return;
    const QPair<QRectF,T> currentPair = m_possibleGarbage.takeFirst();
    QList<T> dataList = m_tree.intersects(currentPair.first.toRect());
    bool found = false;
    foreach ( const T data, dataList )
    {
        // as long as the substyle in question was not found, skip the substyle
        if ( !found )
        {
            if ( data == currentPair.second )
            {
                found = true;
            }
            continue;
        }

        // remove the current pair, if another substyle of the same type,
        // the default style or a named style follows
        if ( data == currentPair.second || data == T() )
        {
            kDebug(36006) << "Storage: removing data at " << currentPair.first << endl;
            m_tree.remove( currentPair.first, currentPair.second );
            break;
        }
    }
}



class CommentStorage : public QObject, public Storage<QString>
{
    Q_OBJECT
public:
    CommentStorage( Sheet* sheet ) : Storage<QString>( sheet )
    {
        connect(m_garbageCollectionInitializationTimer, SIGNAL(timeout()), this, SLOT(garbageCollectionInitialization()));
        connect(m_garbageCollectionTimer, SIGNAL(timeout()), this, SLOT(garbageCollection()));
    }

protected Q_SLOTS:
    virtual void garbageCollectionInitialization() { Storage<QString>::garbageCollectionInitialization(); }
    virtual void garbageCollection() { Storage<QString>::garbageCollection(); }
};



class ConditionsStorage : public QObject, public Storage<Conditions>
{
    Q_OBJECT
public:
    ConditionsStorage( Sheet* sheet ) : Storage<Conditions>( sheet )
    {
        connect(m_garbageCollectionInitializationTimer, SIGNAL(timeout()), this, SLOT(garbageCollectionInitialization()));
        connect(m_garbageCollectionTimer, SIGNAL(timeout()), this, SLOT(garbageCollection()));
    }

protected Q_SLOTS:
    virtual void garbageCollectionInitialization() { Storage<Conditions>::garbageCollectionInitialization(); }
    virtual void garbageCollection() { Storage<Conditions>::garbageCollection(); }
};



class ValidityStorage : public QObject, public Storage<Validity>
{
    Q_OBJECT
public:
    ValidityStorage( Sheet* sheet ) : Storage<Validity>( sheet )
    {
        connect(m_garbageCollectionInitializationTimer, SIGNAL(timeout()), this, SLOT(garbageCollectionInitialization()));
        connect(m_garbageCollectionTimer, SIGNAL(timeout()), this, SLOT(garbageCollection()));
    }

protected Q_SLOTS:
    virtual void garbageCollectionInitialization() { Storage<Validity>::garbageCollectionInitialization(); }
    virtual void garbageCollection() { Storage<Validity>::garbageCollection(); }
};

} // namespace KSpread

#endif // KSPREAD_STORAGE
