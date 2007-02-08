/* This file is part of the KDE project
   Copyright 2006,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "kspread_export.h"

#include "Condition.h"
#include "Doc.h"
#include "Region.h"
#include "RTree.h"
#include "Sheet.h"
#include "Validity.h"

static const int g_garbageCollectionTimeOut = 100;

inline uint qHash( const QPoint& point )
{
    return ( static_cast<uint>( point.x() ) << 16 ) + static_cast<uint>( point.y() );
}

namespace KSpread
{

class Sheet;

/**
 * A custom rectangular storage.
 * Based on an R-Tree data structure.
 * Usable for any kind of data attached to rectangular regions.
 *
 * Acts mainly as a wrapper around the R-Tree data structure to allow a future
 * replacement of this backend. Decorated with some additional features like
 * garbage collection, caching, used area tracking, etc.
 *
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * \since 2.0
 *
 * \note For data assigned to points use PointStorage.
 */
template<typename T>
class KSPREAD_EXPORT RectStorage
{
public:
    explicit RectStorage( Sheet* sheet );
    virtual ~RectStorage();

    /**
     * \return the stored value at the position \p point .
     */
    T contains(const QPoint& point) const;

    /**
     * \return the stored rect/value pair at the position \p point .
     */
    QPair<QRectF, T> containedPair(const QPoint& point) const;

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
    QList< QPair<QRectF,T> > insertRows(int position, int number = 1);

    /**
     * Inserts \p number columns at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,T> > insertColumns(int position, int number = 1);

    /**
     * Deletes \p number rows at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,T> > removeRows(int position, int number = 1);

    /**
     * Deletes \p number columns at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,T> > removeColumns(int position, int number = 1);

    /**
     * Shifts the rows right of \p rect to the right by the width of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,T> > insertShiftRight(const QRect& rect);

    /**
     * Shifts the columns at the bottom of \p rect to the bottom by the height of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF,T> > insertShiftDown(const QRect& rect);

    /**
     * Shifts the rows left of \p rect to the left by the width of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    QList< QPair<QRectF,T> > removeShiftLeft(const QRect& rect);

    /**
     * Shifts the columns on top of \p rect to the top by the height of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    QList< QPair<QRectF,T> > removeShiftUp(const QRect& rect);

protected:
    virtual void triggerGarbageCollection();
    virtual void garbageCollection();

    /**
     * Triggers all necessary actions after a change of \p rect .
     * Calls invalidateCache() and adds the data in
     * \p rect to the list of possible garbage.
     */
    void regionChanged( const QRect& rect );

    /**
     * Invalidates all cached styles lying in \p rect .
     */
    void invalidateCache( const QRect& rect );

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
RectStorage<T>::RectStorage( Sheet* sheet )
    : m_sheet( sheet )
{
}

template<typename T>
RectStorage<T>::~RectStorage()
{
}

template<typename T>
T RectStorage<T>::contains(const QPoint& point) const
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
QPair<QRectF, T> RectStorage<T>::containedPair(const QPoint& point) const
{
    const QList< QPair<QRectF,T> > results = m_tree.intersectingPairs( QRect(point,point) );
    return results.isEmpty() ? qMakePair(QRectF(),T()) : results.last();
}

template<typename T>
QList< QPair<QRectF,T> > RectStorage<T>::undoData(const QRect& rect) const
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
QRect RectStorage<T>::usedArea() const
{
    return m_usedArea.boundingRect();
}

template<typename T>
void RectStorage<T>::insert(const Region& region, const T& _data)
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
        // insert data
        m_tree.insert((*it)->rect(), data);
        regionChanged( (*it)->rect() );
    }
    m_sheet->setRegionPaintDirty( region );
}

template<typename T>
QList< QPair<QRectF,T> > RectStorage<T>::insertRows(int position, int number)
{
    const QRect invalidRect(1,position,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,T> > undoData = m_tree.insertRows(position, number);
    m_sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

template<typename T>
QList< QPair<QRectF,T> > RectStorage<T>::insertColumns(int position, int number)
{
    const QRect invalidRect(position,1,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,T> > undoData = m_tree.insertColumns(position, number);
    m_sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

template<typename T>
QList< QPair<QRectF,T> > RectStorage<T>::removeRows(int position, int number)
{
    const QRect invalidRect(1,position,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,T> > undoData = m_tree.removeRows(position, number);
    m_sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

template<typename T>
QList< QPair<QRectF,T> > RectStorage<T>::removeColumns(int position, int number)
{
    const QRect invalidRect(position,1,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,T> > undoData = m_tree.removeColumns(position, number);
    m_sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

template<typename T>
QList< QPair<QRectF,T> > RectStorage<T>::insertShiftRight(const QRect& rect)
{
    const QRect invalidRect( rect.topLeft(), QPoint(KS_colMax, rect.bottom()) );
    QList< QPair<QRectF,T> > undoData = m_tree.insertShiftRight( rect );
    regionChanged( invalidRect );
    m_sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

template<typename T>
QList< QPair<QRectF,T> > RectStorage<T>::insertShiftDown(const QRect& rect)
{
    const QRect invalidRect( rect.topLeft(), QPoint(rect.right(), KS_rowMax) );
    QList< QPair<QRectF,T> > undoData = m_tree.insertShiftDown( rect );
    regionChanged( invalidRect );
    m_sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

template<typename T>
QList< QPair<QRectF,T> > RectStorage<T>::removeShiftLeft(const QRect& rect)
{
    const QRect invalidRect( rect.topLeft(), QPoint(KS_colMax, rect.bottom()) );
    QList< QPair<QRectF,T> > undoData = m_tree.removeShiftLeft( rect );
    regionChanged( invalidRect );
    m_sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

template<typename T>
QList< QPair<QRectF,T> > RectStorage<T>::removeShiftUp(const QRect& rect)
{
    const QRect invalidRect( rect.topLeft(), QPoint(rect.right(), KS_rowMax) );
    QList< QPair<QRectF,T> > undoData = m_tree.removeShiftUp( rect );
    regionChanged( invalidRect );
    m_sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

template<typename T>
void RectStorage<T>::triggerGarbageCollection()
{
}

template<typename T>
void RectStorage<T>::garbageCollection()
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
        kDebug(36006) << "RectStorage: removing default data at " << currentPair.first << endl;
        m_tree.remove( currentPair.first, currentPair.second );
        triggerGarbageCollection();
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
            kDebug(36006) << "RectStorage: removing data at " << currentPair.first << endl;
            m_tree.remove( currentPair.first, currentPair.second );
            break;
        }
    }
    triggerGarbageCollection();
}

template<typename T>
void RectStorage<T>::regionChanged( const QRect& rect )
{
    if ( m_sheet->doc()->isLoading() )
         return;
    // mark the possible garbage
    m_possibleGarbage += m_tree.intersectingPairs( rect );
    triggerGarbageCollection();
    // invalidate cache
    invalidateCache( rect );
}

template<typename T>
void RectStorage<T>::invalidateCache( const QRect& invRect )
{
//     kDebug(36006) << "StyleStorage: Invalidating " << invRect << endl;
    const QVector<QRect> rects = m_cachedArea.intersected( invRect ).rects();
    m_cachedArea = m_cachedArea.subtracted( invRect );
    foreach ( const QRect& rect, rects )
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



class CommentStorage : public QObject, public RectStorage<QString>
{
    Q_OBJECT
public:
    explicit CommentStorage( Sheet* sheet ) : RectStorage<QString>( sheet ) {}

protected Q_SLOTS:
    virtual void triggerGarbageCollection() { QTimer::singleShot( g_garbageCollectionTimeOut, this, SLOT( garbageCollection() ) ); }
    virtual void garbageCollection() { RectStorage<QString>::garbageCollection(); }
};



class ConditionsStorage : public QObject, public RectStorage<Conditions>
{
    Q_OBJECT
public:
    explicit ConditionsStorage( Sheet* sheet ) : RectStorage<Conditions>( sheet ) {}

protected Q_SLOTS:
    virtual void triggerGarbageCollection() { QTimer::singleShot( g_garbageCollectionTimeOut, this, SLOT( garbageCollection() ) ); }
    virtual void garbageCollection() { RectStorage<Conditions>::garbageCollection(); }
};



class FusionStorage : public QObject, public RectStorage<bool>
{
    Q_OBJECT
public:
    explicit FusionStorage( Sheet* sheet ) : RectStorage<bool>( sheet ) {}

protected Q_SLOTS:
    virtual void triggerGarbageCollection() { QTimer::singleShot( g_garbageCollectionTimeOut, this, SLOT( garbageCollection() ) ); }
    virtual void garbageCollection() { RectStorage<bool>::garbageCollection(); }
};



class ValidityStorage : public QObject, public RectStorage<Validity>
{
    Q_OBJECT
public:
    explicit ValidityStorage( Sheet* sheet ) : RectStorage<Validity>( sheet ) {}

protected Q_SLOTS:
    virtual void triggerGarbageCollection() { QTimer::singleShot( g_garbageCollectionTimeOut, this, SLOT( garbageCollection() ) ); }
    virtual void garbageCollection() { RectStorage<Validity>::garbageCollection(); }
};

} // namespace KSpread

#endif // KSPREAD_STORAGE
