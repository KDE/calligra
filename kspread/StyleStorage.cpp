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

#include <QCache>
#include <QRegion>
#include <QSet>
#include <QTimer>

#include "Global.h"
#include "RTree.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"
#include "StyleManipulators.h"
#include "Storage.h"
#include "StyleStorage.h"

static const int g_maximumCachedStyles = 10000;

using namespace KSpread;

class KDE_NO_EXPORT StyleStorage::Private
{
public:
    Sheet* sheet;
    RTree<SharedSubStyle> tree;
    QSet<int> usedColumns;
    QSet<int> usedRows;
    QRegion usedArea;
    QHash<Style::Key, QList<SharedSubStyle> > subStyles;
    QTimer* garbageCollectionInitializationTimer;
    QTimer* garbageCollectionTimer;
    QList< QPair<QRectF,SharedSubStyle> > possibleGarbage;
    QCache<QPoint, Style> cache;
    QRegion cachedArea;
};

StyleStorage::StyleStorage( Sheet* sheet )
    : QObject( sheet )
    , d(new Private)
{
    d->sheet = sheet;
    d->garbageCollectionInitializationTimer = new QTimer(this);
    connect(d->garbageCollectionInitializationTimer, SIGNAL(timeout()), this, SLOT(garbageCollectionInitialization()));
    d->garbageCollectionInitializationTimer->start(g_garbageCollectionTimeOut);
    d->garbageCollectionTimer = new QTimer(this);
    connect(d->garbageCollectionTimer, SIGNAL(timeout()), this, SLOT(garbageCollection()));
    d->garbageCollectionTimer->start();
    d->cache.setMaxCost( g_maximumCachedStyles );
}

StyleStorage::~StyleStorage()
{
    delete d->garbageCollectionInitializationTimer;
    delete d->garbageCollectionTimer;
    delete d;
}

Style StyleStorage::contains(const QPoint& point) const
{
    // first, lookup point in the cache
    if ( d->cache.contains( point ) )
    {
//         kDebug(36006) << "StyleStorage: Using cached style for " << cellName << endl;
        return *d->cache.object( point );
    }
    // not found, lookup in the tree
    QList<SharedSubStyle> subStyles = d->tree.contains(point);
    if ( subStyles.isEmpty() )
        return *styleManager()->defaultStyle();
    Style* style = new Style();
    (*style) = composeStyle( subStyles );
    // insert style into the cache
    d->cache.insert( point, style );
    d->cachedArea += QRect( point, point );
    return *style;
}

Style StyleStorage::contains(const QRect& rect) const
{
    QList<SharedSubStyle> subStyles = d->tree.contains(rect);
    return composeStyle( subStyles );
}

Style StyleStorage::intersects(const QRect& rect) const
{
    QList<SharedSubStyle> subStyles = d->tree.intersects(rect);
    return composeStyle( subStyles );
}

QList< QPair<QRectF,SharedSubStyle> > StyleStorage::undoData(const QRect& rect) const
{
    QList< QPair<QRectF,SharedSubStyle> > result = d->tree.intersectingPairs(rect);
    for ( int i = 0; i < result.count(); ++i )
    {
        // trim the rects
        result[i].first = result[i].first.intersected( rect );
    }
    return result;
}

QRect StyleStorage::usedArea() const
{
    return d->usedArea.boundingRect();
}

int StyleStorage::nextColumn( int column ) const
{
    QList<int> list = d->usedColumns.toList();
    qSort(list);
    QList<int>::Iterator it(list.begin());
    while ( it != list.end() && *it <= column )
        ++it;
    return ( it != list.end() ) ? *it : 0;
}

int StyleStorage::nextRow( int row ) const
{
    QList<int> list = d->usedRows.toList();
    qSort(list);
    QList<int>::Iterator it(list.begin());
    while ( it != list.end() && *it <= row )
        ++it;
    return ( it != list.end() ) ? *it : 0;
}

int StyleStorage::nextStyleRight( int column, int row ) const
{
    QRegion region = d->usedArea & QRect( column + 1, row, KS_colMax, 1 );
    QList<int> usedColumns;
    foreach ( const QRect& rect, region.rects() )
    {
        for ( int col = rect.left(); col <= rect.right(); ++col )
        {
            usedColumns.append( col );
        }
    }
    if ( usedColumns.isEmpty() )
        return 0;
    qSort(usedColumns);
    return usedColumns.first();
}

void StyleStorage::insert(const QRect& rect, const SharedSubStyle& subStyle)
{
//     kDebug(36006) << "StyleStorage: inserting " << subStyle->type() << " into " << rect << endl;
    // invalidate the affected, cached styles
    invalidateCache( rect );
    // lookup already used substyles
    typedef const QList< SharedSubStyle> StoredSubStyleList;
    StoredSubStyleList& storedSubStyles( d->subStyles.value(subStyle->type()) );
    StoredSubStyleList::ConstIterator end(storedSubStyles.end());
    for ( StoredSubStyleList::ConstIterator it(storedSubStyles.begin()); it != end; ++it )
    {
        if ( Style::compare( subStyle.data(), (*it).data() ) )
        {
//             kDebug(36006) << "[REUSING EXISTING SUBSTYLE]" << endl;
            d->tree.insert(rect, *it);
            return;
        }
    }
    // insert substyle and add to the used substyle list
    d->tree.insert(rect, subStyle);
    d->subStyles[subStyle->type()].append(subStyle);
}

void StyleStorage::insert(const Region& region, const Style& style)
{
    if ( style.isEmpty() )
        return;
    foreach ( const SharedSubStyle& subStyle, style.subStyles() )
    {
        const bool isDefault = subStyle->type() == Style::DefaultStyleKey;
        Region::ConstIterator end(region.constEnd());
        for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
        {
            // keep track of the used area
            if ( (*it)->isColumn() )
            {
                for ( int i = (*it)->rect().left(); i <= (*it)->rect().right(); ++i )
                {
                    if ( isDefault )
                        d->usedColumns.remove( i );
                    else
                        d->usedColumns.insert( i );
                }
            }
            else if ( (*it)->isRow() )
            {
                for ( int i = (*it)->rect().top(); i <= (*it)->rect().bottom(); ++i )
                {
                    if ( isDefault )
                        d->usedRows.remove( i );
                    else
                        d->usedRows.insert( i );
                }
            }
            if ( isDefault )
                d->usedArea -= (*it)->rect();
            else
                d->usedArea += (*it)->rect();

            // invalidate the affected, cached styles
            invalidateCache( (*it)->rect() );
            // insert substyle
            insert((*it)->rect(), subStyle);
        }
    }
    d->sheet->setRegionPaintDirty( region );
}

QList< QPair<QRectF,SharedSubStyle> > StyleStorage::insertRows(int position, int number)
{
    const QRect invalidRect(1,position,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,SharedSubStyle> > undoData = d->tree.insertRows(position, number);
    d->sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

QList< QPair<QRectF,SharedSubStyle> > StyleStorage::insertColumns(int position, int number)
{
    const QRect invalidRect(position,1,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,SharedSubStyle> > undoData = d->tree.insertColumns(position, number);
    d->sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

QList< QPair<QRectF,SharedSubStyle> > StyleStorage::deleteRows(int position, int number)
{
    const QRect invalidRect(1,position,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,SharedSubStyle> > undoData = d->tree.deleteRows(position, number);
    d->sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

QList< QPair<QRectF,SharedSubStyle> > StyleStorage::deleteColumns(int position, int number)
{
    const QRect invalidRect(position,1,KS_colMax,KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,SharedSubStyle> > undoData = d->tree.deleteColumns(position, number);
    d->sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

QList< QPair<QRectF,SharedSubStyle> > StyleStorage::shiftRows( const QRect& rect )
{
    const QRect invalidRect( rect.topLeft(), QPoint(KS_colMax, rect.bottom()) );
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,SharedSubStyle> > undoData = d->tree.shiftRows( rect );
    d->sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

QList< QPair<QRectF,SharedSubStyle> > StyleStorage::shiftColumns( const QRect& rect )
{
    const QRect invalidRect( rect.topLeft(), QPoint(rect.right(), KS_rowMax) );
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,SharedSubStyle> > undoData = d->tree.shiftColumns( rect );
    d->sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

QList< QPair<QRectF,SharedSubStyle> > StyleStorage::unshiftRows( const QRect& rect )
{
    const QRect invalidRect( rect.topLeft(), QPoint(KS_colMax, rect.bottom()) );
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,SharedSubStyle> > undoData = d->tree.unshiftRows( rect );
    d->sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

QList< QPair<QRectF,SharedSubStyle> > StyleStorage::unshiftColumns( const QRect& rect )
{
    const QRect invalidRect( rect.topLeft(), QPoint(rect.right(), KS_rowMax) );
    // invalidate the affected, cached styles
    invalidateCache( invalidRect );
    QList< QPair<QRectF,SharedSubStyle> > undoData = d->tree.unshiftColumns( rect );
    d->sheet->setRegionPaintDirty( Region(invalidRect) );
    return undoData;
}

void StyleStorage::garbageCollectionInitialization()
{
    // last garbage collection finished?
    if ( !d->possibleGarbage.isEmpty() )
        return;
    d->possibleGarbage = d->tree.intersectingPairs( QRect( 1, 1, KS_colMax, KS_rowMax ) );
}

void StyleStorage::garbageCollection()
{
    // any possible garbage left?
    if ( d->possibleGarbage.isEmpty() )
        return;

    const QPair<QRectF, SharedSubStyle> currentPair = d->possibleGarbage.takeFirst();

    // check wether the named style still exists
    if ( currentPair.second->type() == Style::NamedStyleKey &&
         !styleManager()->style( static_cast<const NamedStyle*>(currentPair.second.data())->name ) )
    {
        kDebug(36006) << "StyleStorage: removing " << currentPair.second->debugData()
                        << " at " << currentPair.first
                        << ", used " << currentPair.second->ref << " times" << endl;
        d->tree.remove( currentPair.first, currentPair.second );
        d->subStyles[currentPair.second->type()].removeAll( currentPair.second );
        return; // already done
    }

    typedef QPair<QRectF,SharedSubStyle> SharedSubStylePair;
    QList<SharedSubStylePair> pairs = d->tree.intersectingPairs(currentPair.first.toRect());
    if ( pairs.isEmpty() ) // actually never true, just for sanity
         return;

    // check wether the default style is placed first
    if ( currentPair.second->type() == Style::DefaultStyleKey &&
         pairs[0].second->type() == Style::DefaultStyleKey &&
         pairs[0].first == currentPair.first )
    {
        kDebug(36006) << "StyleStorage: removing default style"
                        << " at " << currentPair.first
                        << ", used " << currentPair.second->ref << " times" << endl;
        d->tree.remove( currentPair.first, currentPair.second );
        return; // already done
    }

    bool found = false;
    foreach( const SharedSubStylePair& pair, pairs )
    {
        // as long as the substyle in question was not found, skip the substyle
        if ( !found )
        {
            if ( pair.first == currentPair.first &&
                 Style::compare( pair.second.data(), currentPair.second.data() ) )
            {
                found = true;
            }
            continue;
        }

        // remove the current pair, if another substyle of the same type,
        // the default style or a named style follows and the rectangle
        // is completely convered
        if ( ( pair.second->type() == currentPair.second->type() ||
               pair.second->type() == Style::DefaultStyleKey ||
               pair.second->type() == Style::NamedStyleKey ) &&
             pair.first.contains( currentPair.first ) )
        {
            kDebug(36006) << "StyleStorage: removing " << currentPair.second->debugData()
                          << " at " << currentPair.first
                          << ", used " << currentPair.second->ref << "times" << endl;
            d->tree.remove( currentPair.first, currentPair.second );
#if 0
            kDebug(36006) << "StyleStorage: usage of " << currentPair.second->debugData() << " is " << currentPair.second->ref << endl;
            // FIXME Stefan: The usage of substyles used once should be
            //               two (?) here, not more. Why is this not the case?
            //               The shared pointers are used by:
            //               a) the tree
            //               b) the reusage list (where it should be removed)
            //               c) the cached styles (!)
            //               d) the undo data of operations (!)
            if ( currentPair.second->ref == 2 )
            {
                kDebug(36006) << "StyleStorage: removing " << currentPair.second << " from the used subStyles" << endl;
                d->subStyles[currentPair.second->type()].removeAll( currentPair.second );
            }
#endif
            break;
        }
    }
}

void StyleStorage::invalidateCache( const QRect& rect )
{
//     kDebug(36006) << "StyleStorage: Invalidating " << rect << endl;
    const QRegion region = d->cachedArea.intersected( rect );
    d->cachedArea = d->cachedArea.subtracted( rect );
    foreach ( const QRect& rect, region.rects() )
    {
        for ( int col = rect.left(); col <= rect.right(); ++col )
        {
            for ( int row = rect.top(); row <= rect.bottom(); ++row )
            {
//                 kDebug(36006) << "StyleStorage: Removing cached style for " << Cell::name( col, row ) << endl;
                d->cache.remove( QPoint( col, row ) ); // also deletes it
            }
        }
    }
}

Style StyleStorage::composeStyle( const QList<SharedSubStyle>& subStyles ) const
{
    Style style;
    for ( int i = 0; i < subStyles.count(); ++i )
    {
        if ( subStyles[i]->type() == Style::DefaultStyleKey )
            style = *styleManager()->defaultStyle();
        else if ( subStyles[i]->type() == Style::NamedStyleKey )
        {
            style.clear();
            const CustomStyle* namedStyle = styleManager()->style( static_cast<const NamedStyle*>(subStyles[i].data())->name );
            if ( namedStyle )
            {
                // first, load the attributes of the parent style(s)
                QList<CustomStyle*> parentStyles;
                CustomStyle* parentStyle = styleManager()->style( namedStyle->parentName() );
//                 if ( parentStyle )
//                     kDebug(36006) << "StyleStorage: " << namedStyle->name() << "'s parent = " << parentStyle->name() << endl;
                while ( parentStyle )
                {
//                     kDebug(36006) << "StyleStorage: " << parentStyle->name() << "'s parent = " << parentStyle->parentName() << endl;
                    parentStyles.prepend( parentStyle );
                    // FIXME Stefan: Make sure, that "Default" does not inherit "Default"! OOo default style!
                    if ( parentStyle->name() == "Default" )
                        break;
                    parentStyle = styleManager()->style( parentStyle->parentName() );
                }
                for ( int i = 0; i < parentStyles.count(); ++i )
                    style.merge( *parentStyles[i] );
                // second, merge the other attributes in
                style.merge( *namedStyle );
                // not the default anymore
                style.clearAttribute( Style::DefaultStyleKey );
            }
        }
        else
        {
            // insert the substyle
            style.insertSubStyle( subStyles[i] );
            // not the default anymore
            style.clearAttribute( Style::DefaultStyleKey );
            // and not a named style anymore
            style.clearAttribute( Style::NamedStyleKey);
        }
    }
    return style;
}

StyleManager* StyleStorage::styleManager() const
{
    return d->sheet->doc()->styleManager();
}

#include "StyleStorage.moc"
