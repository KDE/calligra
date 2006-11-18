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

#include <QRegion>
#include <QSet>
#include <QTimer>

#include "Global.h"
#include "RTree.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"
#include "StyleManipulators.h"

#include "StyleStorage.h"

static const int g_garbageCollectionTimeOut = 60000; // one minute

using namespace KSpread;

class KDE_NO_EXPORT StyleStorage::Private
{
public:
    Sheet* sheet;
    RTree<QSharedDataPointer<SubStyle> > tree;
    StyleManager* styleManager;
    QSet<int> usedColumns;
    QSet<int> usedRows;
    QRegion usedArea;
    QHash<Style::Key, QList<QSharedDataPointer<SubStyle> > > subStyles;
    QTimer* garbageCollectionInitializationTimer;
    QTimer* garbageCollectionTimer;
    QList< QPair<QRectF,QSharedDataPointer<SubStyle> > > possibleGarbage;
};

StyleStorage::StyleStorage( Sheet* sheet )
    : QObject( sheet )
    , d(new Private)
{
    d->sheet = sheet;
    d->styleManager = 0;
    d->garbageCollectionInitializationTimer = new QTimer(this);
    connect(d->garbageCollectionInitializationTimer, SIGNAL(timeout()), this, SLOT(garbageCollectionInitialization()));
    d->garbageCollectionInitializationTimer->start(g_garbageCollectionTimeOut);
    d->garbageCollectionTimer = new QTimer(this);
    connect(d->garbageCollectionTimer, SIGNAL(timeout()), this, SLOT(garbageCollection()));
    d->garbageCollectionTimer->start();
}

StyleStorage::~StyleStorage()
{
    delete d->garbageCollectionInitializationTimer;
    delete d->garbageCollectionTimer;
    delete d;
}

void StyleStorage::setStyleManager(StyleManager* manager)
{
    d->styleManager = manager;
}

Style StyleStorage::contains(const QPoint& point) const
{
    Q_ASSERT(d->styleManager);
    QList<QSharedDataPointer<SubStyle> > subStyles = d->tree.contains(point);
    Style style;
    foreach ( const QSharedDataPointer<SubStyle> subStyle, subStyles )
    {
        style.insertSubStyle( subStyle );
    }
    return subStyles.isEmpty() ? *d->styleManager->defaultStyle() : style;
}

Style StyleStorage::contains(const QRect& rect) const
{
    Q_ASSERT(d->styleManager);
    QList<QSharedDataPointer<SubStyle> > subStyles = d->tree.contains(rect);
    Style style;
    foreach ( const QSharedDataPointer<SubStyle> subStyle, subStyles )
    {
        style.insertSubStyle( subStyle );
    }
    return subStyles.isEmpty() ? *d->styleManager->defaultStyle() : style;
}

Style StyleStorage::intersects(const QRect& rect) const
{
    Q_ASSERT(d->styleManager);
    QList<QSharedDataPointer<SubStyle> > subStyles = d->tree.intersects(rect);
    Style style;
    foreach ( const QSharedDataPointer<SubStyle> subStyle, subStyles )
    {
        style.insertSubStyle( subStyle );
    }
    return subStyles.isEmpty() ? *d->styleManager->defaultStyle() : style;
}

QList< QPair<QRectF,QSharedDataPointer<SubStyle> > > StyleStorage::undoData(const QRect& rect) const
{
    QList< QPair<QRectF,QSharedDataPointer<SubStyle> > > result = d->tree.intersectingPairs(rect);
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
    foreach ( QRect rect, region.rects() )
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

void StyleStorage::insert(const QRect& rect, const QSharedDataPointer<SubStyle> subStyle)
{
//     kDebug() << "StyleStorage: inserting " << subStyle->type() << " into " << rect << endl;
    // lookup already used substyles
    typedef const QList< QSharedDataPointer<SubStyle> > StoredSubStyleList;
    StoredSubStyleList& storedSubStyles( d->subStyles.value(subStyle->type()) );
    StoredSubStyleList::ConstIterator end(storedSubStyles.end());
    for ( StoredSubStyleList::ConstIterator it(storedSubStyles.begin()); it != end; ++it )
    {
        if ( Style::compare( subStyle.data(), (*it).data() ) )
        {
//             kDebug() << "[REUSING EXISTING SUBSTYLE]" << endl;
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
    foreach ( const QSharedDataPointer<SubStyle> subStyle, style.subStyles() )
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

            // insert substyle
            insert((*it)->rect(), subStyle);
        }
    }
}

void StyleStorage::insertRows(int position, int number)
{
    d->tree.insertRows(position, number);
    d->sheet->addLayoutDirtyRegion( Region(QRect(1,position,KS_colMax,KS_rowMax)) );
}

void StyleStorage::insertColumns(int position, int number)
{
    d->tree.insertColumns(position, number);
    d->sheet->addLayoutDirtyRegion( Region(QRect(position,1,KS_colMax,KS_rowMax)) );
}

void StyleStorage::deleteRows(int position, int number)
{
    d->tree.deleteRows(position, number);
    d->sheet->addLayoutDirtyRegion( Region(QRect(1,position,KS_colMax,KS_rowMax)) );
}

void StyleStorage::deleteColumns(int position, int number)
{
    d->tree.deleteColumns(position, number);
    d->sheet->addLayoutDirtyRegion( Region(QRect(position,1,KS_colMax,KS_rowMax)) );
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
    const QPair<QRectF, QSharedDataPointer<SubStyle> > currentPair = d->possibleGarbage.takeFirst();
    QList<QSharedDataPointer<SubStyle> > subStyles = d->tree.intersects(currentPair.first.toRect());
    bool found = false;
    foreach ( const QSharedDataPointer<SubStyle> subStyle, subStyles )
    {
        // as long as the substyle in question was not found, skip the substyle
        if ( !found )
        {
            if ( Style::compare( subStyle.data(), currentPair.second.data() ) )
            {
                found = true;
            }
            continue;
        }

        // remove the current pair, if another substyle of the same type,
        // the default style or a named style follows
        if ( subStyle->type() == currentPair.second->type() ||
             subStyle->type() == Style::DefaultStyleKey ||
             subStyle->type() == Style::NamedStyleKey )
        {
            kDebug(36006) << "StyleStorage: removing " << currentPair.second << " at " << currentPair.first << endl;
            d->tree.remove( currentPair.first, currentPair.second );
            kDebug(36006) << "StyleStorage: usage of " << currentPair.second << " is " << currentPair.second->ref << endl;
            // FIXME Stefan: The usage of substyles used once should be
            //               two (?) here, not more. Why is this not the case?
            //               The shared pointers are used by:
            //               a) the tree
            //               b) the reusage list (where it should be removed)
            //               c) the cached styles (!)
            if ( currentPair.second->ref == 2 )
            {
                kDebug(36006) << "StyleStorage: removing " << currentPair.second << " from the used subStyles" << endl;
                d->subStyles[currentPair.second->type()].removeAll( currentPair.second );
            }
            break;
        }
    }
}

#include "StyleStorage.moc"
