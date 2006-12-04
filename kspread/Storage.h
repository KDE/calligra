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

#include <QCache>
#include <QTimer>

#include <koffice_export.h>

#include "Condition.h"
#include "Region.h"
#include "RTree.h"
#include "Sheet.h"
#include "Validity.h"

static const int g_garbageCollectionTimeOut = 60000; // one minute

inline uint qHash( const QPoint& point )
{
    return ( static_cast<uint>( point.x() ) << 16 ) + static_cast<uint>( point.y() );
}

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
    explicit Storage( Sheet* sheet );
    virtual ~Storage();

    /**
     * \return the stored value at the position \p point .
     */
    T contains(const QPoint& point) const;

    QList< QPair<QRectF, T> > undoData(const QRect& rect) const;

    /**
     * Returns the area, which got data attached.
     * \return the area using data
     */
    QRect usedArea() const;

    /**
     * Assigns \p data to \p region .
     */
    void insert(const Region& region, const T& data);

    /**
     * Inserts \p number rows at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    void insertRows(int position, int number = 1);

    /**
     * Inserts \p number columns at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    void insertColumns(int position, int number = 1);

    /**
     * Deletes \p number rows at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    void deleteRows(int position, int number = 1);

    /**
     * Deletes \p number columns at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    void deleteColumns(int position, int number = 1);

    /**
     * Shifts the rows right of \p rect to the right by the width of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,T> > shiftRows(const QRect& rect);

    /**
     * Shifts the columns at the bottom of \p rect to the bottom by the height of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,T> > shiftColumns(const QRect& rect);

    /**
     * Shifts the rows left of \p rect to the left by the width of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    QList< QPair<QRectF,T> > unshiftRows(const QRect& rect);

    /**
     * Shifts the columns on top of \p rect to the top by the height of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    QList< QPair<QRectF,T> > unshiftColumns(const QRect& rect);

protected:
    virtual void garbageCollectionInitialization();
    virtual void garbageCollection();

    /**
     * Invalidates all cached styles lying in \p rect .
     */
    void invalidateCache( const QRect& rect );

    QTimer* m_garbageCollectionInitializationTimer;
    QTimer* m_garbageCollectionTimer;

private:
    Sheet* m_sheet;
    RTree<T> m_tree;
    QRegion m_usedArea;
    QList< QPair<QRectF,T> > m_possibleGarbage;
    QList<T> m_storedData;
    mutable QCache<QPoint,T> m_cache;
    mutable QRegion m_cachedArea;
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
T Storage<T>::contains(const QPoint& point) const
{
    // first, lookup point in the cache
    if ( m_cache.contains( point ) )
    {
        return *m_cache.object( point );
    }
    // not found, lookup in the tree
    QList<T> results = m_tree.contains(point);
    T data = results.isEmpty() ? T() : results.last();
    // insert style into the cache
    m_cache.insert( point, new T(data) );
    m_cachedArea += QRect( point, point );
    return data;
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
QRect Storage<T>::usedArea() const
{
    return m_usedArea.boundingRect();
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
        // keep track of the used area
        if ( data == T() )
            m_usedArea -= (*it)->rect();
        else
            m_usedArea += (*it)->rect();
        // invalidate the affected, cached styles
        invalidateCache( (*it)->rect() );
        // insert data
        m_tree.insert((*it)->rect(), data);
    }
}

template<typename T>
void Storage<T>::insertRows(int position, int number)
{
    const QRect invalidRect(1,position,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    m_tree.insertRows(position, number);
    m_sheet->addLayoutDirtyRegion( Region(invalidRect) );
}

template<typename T>
void Storage<T>::insertColumns(int position, int number)
{
    const QRect invalidRect(position,1,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    m_tree.insertColumns(position, number);
    m_sheet->addLayoutDirtyRegion( Region(invalidRect) );
}

template<typename T>
void Storage<T>::deleteRows(int position, int number)
{
    const QRect invalidRect(1,position,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    m_tree.deleteRows(position, number);
    m_sheet->addLayoutDirtyRegion( Region(invalidRect) );
}

template<typename T>
void Storage<T>::deleteColumns(int position, int number)
{
    const QRect invalidRect(position,1,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    m_tree.deleteColumns(position, number);
    m_sheet->addLayoutDirtyRegion( Region(invalidRect) );
}

template<typename T>
QList< QPair<QRectF,T> > Storage<T>::shiftRows(const QRect& rect)
{
    const QRect invalidRect( rect.topLeft(), QPoint(KS_colMax, rect.bottom()) );
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,T> > undoData = m_tree.shiftRows( rect );
    m_sheet->addLayoutDirtyRegion( Region(invalidRect) );
    return undoData;
}

template<typename T>
QList< QPair<QRectF,T> > Storage<T>::shiftColumns(const QRect& rect)
{
    const QRect invalidRect( rect.topLeft(), QPoint(rect.right(), KS_rowMax) );
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,T> > undoData = m_tree.shiftColumns( rect );
    m_sheet->addLayoutDirtyRegion( Region(invalidRect) );
    return undoData;
}

template<typename T>
QList< QPair<QRectF,T> > Storage<T>::unshiftRows(const QRect& rect)
{
    const QRect invalidRect( rect.topLeft(), QPoint(KS_colMax, rect.bottom()) );
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,T> > undoData = m_tree.unshiftRows( rect );
    m_sheet->addLayoutDirtyRegion( Region(invalidRect) );
    return undoData;
}

template<typename T>
QList< QPair<QRectF,T> > Storage<T>::unshiftColumns(const QRect& rect)
{
    const QRect invalidRect( rect.topLeft(), QPoint(rect.right(), KS_rowMax) );
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,T> > undoData = m_tree.unshiftColumns( rect );
    m_sheet->addLayoutDirtyRegion( Region(invalidRect) );
    return undoData;
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
    typedef QPair<QRectF,T> DataPair;
    QList<DataPair> pairs = m_tree.intersectingPairs(currentPair.first.toRect());
    if ( pairs.isEmpty() ) // actually never true, just for sanity
         return;

    // check wether the default style is placed first
    if ( currentPair.second == T() && pairs[0].second == T() && pairs[0].first == currentPair.first )
    {
        kDebug(36006) << "Storage: removing default data at " << currentPair.first << endl;
        m_tree.remove( currentPair.first, currentPair.second );
        return; // already done
    }

    bool found = false;
    foreach ( const DataPair pair, pairs )
    {
        // as long as the substyle in question was not found, skip the substyle
        if ( !found )
        {
            if ( pair.first == currentPair.first && pair.second == currentPair.second )
            {
                found = true;
            }
            continue;
        }

        // remove the current pair, if another substyle of the same type,
        // the default style or a named style follows and the rectangle
        // is completely convered
        if ( ( pair.second == currentPair.second || pair.second == T() ) &&
             pair.first.contains( currentPair.first ) )
        {
            kDebug(36006) << "Storage: removing data at " << currentPair.first << endl;
            m_tree.remove( currentPair.first, currentPair.second );
            break;
        }
    }
}

template<typename T>
void Storage<T>::invalidateCache( const QRect& rect )
{
//     kDebug(36006) << "StyleStorage: Invalidating " << rect << endl;
    const QRegion region = m_cachedArea.intersected( rect );
    m_cachedArea = m_cachedArea.subtracted( rect );
    foreach ( const QRect& rect, region.rects() )
    {
        for ( int col = rect.left(); col <= rect.right(); ++col )
        {
            for ( int row = rect.top(); row <= rect.bottom(); ++row )
            {
//                 kDebug(36006) << "StyleStorage: Removing cached style for " << Cell::name( col, row ) << endl;
                m_cache.remove( QPoint( col, row ) ); // also deletes it
            }
        }
    }
}



class CommentStorage : public QObject, public Storage<QString>
{
    Q_OBJECT
public:
    explicit CommentStorage( Sheet* sheet ) : Storage<QString>( sheet )
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
    explicit ConditionsStorage( Sheet* sheet ) : Storage<Conditions>( sheet )
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
    explicit ValidityStorage( Sheet* sheet ) : Storage<Validity>( sheet )
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
