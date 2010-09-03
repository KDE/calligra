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

// Local
#include "StyleStorage.h"

#include <QCache>
#include <QRegion>
#include <QTimer>

#include "Global.h"
#include "Map.h"
#include "OdfSavingContext.h"
#include "RTree.h"
#include "Style.h"
#include "StyleManager.h"
#include "RectStorage.h"

static const int g_maximumCachedStyles = 10000;

using namespace KSpread;

class KDE_NO_EXPORT StyleStorage::Private
{
public:
    Map* map;
    RTree<SharedSubStyle> tree;
    QMap<int, bool> usedColumns; // FIXME Stefan: Use QList and qUpperBound() for insertion.
    QMap<int, bool> usedRows;
    QRegion usedArea;
    QHash<Style::Key, QList<SharedSubStyle> > subStyles;
    QMap<int, QPair<QRectF, SharedSubStyle> > possibleGarbage;
    QCache<QPoint, Style> cache;
    QRegion cachedArea;
};

StyleStorage::StyleStorage(Map* map)
        : QObject(map)
        , d(new Private)
{
    d->map = map;
    d->cache.setMaxCost(g_maximumCachedStyles);
}

StyleStorage::StyleStorage(const StyleStorage& other)
        : QObject(other.d->map)
        , d(new Private)
{
    d->map = other.d->map;
    d->tree = other.d->tree;
    d->usedColumns = other.d->usedColumns;
    d->usedRows = other.d->usedRows;
    d->usedArea = other.d->usedArea;
    d->subStyles = other.d->subStyles;
    // the other member variables are temporary stuff
}

StyleStorage::~StyleStorage()
{
    delete d;
}

Style StyleStorage::contains(const QPoint& point) const
{
    if (!d->usedArea.contains(point) && !d->usedColumns.contains(point.x()) && !d->usedRows.contains(point.y()))
        return *styleManager()->defaultStyle();
    // first, lookup point in the cache
    if (d->cache.contains(point)) {
//         kDebug(36006) <<"StyleStorage: Using cached style for" << cellName;
        return *d->cache.object(point);
    }
    // not found, lookup in the tree
    QList<SharedSubStyle> subStyles = d->tree.contains(point);
    if (subStyles.isEmpty())
        return *styleManager()->defaultStyle();
    Style* style = new Style();
    (*style) = composeStyle(subStyles);
    // insert style into the cache
    d->cache.insert(point, style);
    d->cachedArea += QRect(point, point);
    return *style;
}

Style StyleStorage::contains(const QRect& rect) const
{
    QList<SharedSubStyle> subStyles = d->tree.contains(rect);
    return composeStyle(subStyles);
}

Style StyleStorage::intersects(const QRect& rect) const
{
    QList<SharedSubStyle> subStyles = d->tree.intersects(rect);
    return composeStyle(subStyles);
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::undoData(const Region& region) const
{
    QList< QPair<QRectF, SharedSubStyle> > result;
    Region::ConstIterator end = region.constEnd();
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
        const QRect rect = (*it)->rect();
        QList< QPair<QRectF, SharedSubStyle> > pairs = d->tree.intersectingPairs(rect).values();
        for (int i = 0; i < pairs.count(); ++i) {
            // trim the rects
            pairs[i].first = pairs[i].first.intersected(rect);
        }
        // Always a default subStyle first, even if there are no pairs.
        result << qMakePair(QRectF(rect), SharedSubStyle()) << pairs;
    }
    return result;
}

QRect StyleStorage::usedArea() const
{
    if (d->usedArea.isEmpty())
        return QRect(1, 1, 0, 0);
    return QRect(QPoint(1, 1), d->usedArea.boundingRect().bottomRight());
}

void StyleStorage::saveOdfCreateDefaultStyles(int& maxCols, int& maxRows, OdfSavingContext& tableContext) const
{
#if 0 // TODO
    // If we have both, column and row styles, we can take the short route.
    if (!d->usedColumns.isEmpty() && !d->usedRows.isEmpty()) {
        for (int i = 0; i < d->usedColumns.count(); ++i) {
            const int col = d->usedColumns[i];
            tableContext.columnDefaultStyles[col].insertSubStyle(contains(QRect(col, 1, 1, KS_rowMax)));
        }
        for (int i = 0; i < d->usedColumns.count(); ++i) {
            const int row = d->usedRow[i];
            tableContext.rowDefaultStyles[row].insertSubStyle(contains(QRect(1, row, KS_colMax, 1)));
        }
        return;
    }
#endif
    const QRect sheetRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax));
    if (d->usedColumns.count() != 0) {
        maxCols = qMax(maxCols, (--d->usedColumns.constEnd()).key());
        maxRows = KS_rowMax;
    }
    if (d->usedRows.count() != 0) {
        maxCols = KS_colMax;
        maxRows = qMax(maxRows, (--d->usedRows.constEnd()).key());
    }
    const QList< QPair<QRectF, SharedSubStyle> > pairs = d->tree.intersectingPairs(sheetRect).values();
    for (int i = 0; i < pairs.count(); ++i) {
        const QRect rect = pairs[i].first.toRect();
        // column default cell styles
        // Columns have no content. Prefer them over rows for the default cell styles.
        if (rect.top() == 1 && rect.bottom() == maxRows) {
            for (int col = rect.left(); col <= rect.right(); ++col) {
                if (pairs[i].second.data()->type() == Style::DefaultStyleKey)
                    tableContext.columnDefaultStyles.remove(col);
                else
                    tableContext.columnDefaultStyles[col].insertSubStyle(pairs[i].second);
            }
        }
        // row default cell styles
        else if (rect.left() == 1 && rect.right() == maxCols) {
            for (int row = rect.top(); row <= rect.bottom(); ++row) {
                if (pairs[i].second.data()->type() == Style::DefaultStyleKey)
                    tableContext.rowDefaultStyles.remove(row);
                else
                    tableContext.rowDefaultStyles[row].insertSubStyle(pairs[i].second);
            }
        }
    }
}

int StyleStorage::nextColumnStyleIndex(int column) const
{
    QMap<int, bool>::iterator it = d->usedColumns.upperBound(column + 1);
    return (it == d->usedColumns.end()) ? 0 : it.key();
}

int StyleStorage::nextRowStyleIndex(int row) const
{
    QMap<int, bool>::iterator it = d->usedRows.upperBound(row + 1);
    return (it == d->usedRows.end()) ? 0 : it.key();
}

int StyleStorage::firstColumnIndexInRow(int row) const
{
    const QRect rect = (d->usedArea & QRect(QPoint(1, row), QPoint(KS_colMax, row))).boundingRect();
    return rect.isNull() ? 0 : rect.left();
}

int StyleStorage::nextColumnIndexInRow(int column, int row) const
{
    const QRect rect = (d->usedArea & QRect(QPoint(column + 1, row), QPoint(KS_colMax, row))).boundingRect();
    return rect.isNull() ? 0 : rect.left();
}

void StyleStorage::insert(const QRect& rect, const SharedSubStyle& subStyle)
{
//     kDebug(36006) <<"StyleStorage: inserting" << SubStyle::name(subStyle->type()) <<" into" << rect;
    // keep track of the used area
    const bool isDefault = subStyle->type() == Style::DefaultStyleKey;
    if (rect.top() == 1 && rect.bottom() >= KS_rowMax) {
        for (int i = rect.left(); i <= rect.right(); ++i) {
            if (isDefault)
                d->usedColumns.remove(i);
            else
                d->usedColumns.insert(i, true);
        }
        if (isDefault)
            d->usedArea -= rect;
    } else if (rect.left() == 1 && rect.right() >= KS_colMax) {
        for (int i = rect.top(); i <= rect.bottom(); ++i) {
            if (isDefault)
                d->usedRows.remove(i);
            else
                d->usedRows.insert(i, true);
        }
        if (isDefault)
            d->usedArea -= rect;
    } else {
        if (isDefault)
            d->usedArea -= rect;
        else
            d->usedArea += rect;
    }

    // lookup already used substyles
    typedef const QList< SharedSubStyle> StoredSubStyleList;
    StoredSubStyleList& storedSubStyles(d->subStyles.value(subStyle->type()));
    StoredSubStyleList::ConstIterator end(storedSubStyles.end());
    for (StoredSubStyleList::ConstIterator it(storedSubStyles.begin()); it != end; ++it) {
        if (Style::compare(subStyle.data(), (*it).data())) {
//             kDebug(36006) <<"[REUSING EXISTING SUBSTYLE]";
            d->tree.insert(rect, *it);
            regionChanged(rect);
            return;
        }
    }
    // insert substyle and add to the used substyle list
    d->tree.insert(rect, subStyle);
    d->subStyles[subStyle->type()].append(subStyle);
    regionChanged(rect);
}

void StyleStorage::insert(const Region& region, const Style& style)
{
    if (style.isEmpty())
        return;
    foreach(const SharedSubStyle& subStyle, style.subStyles()) {
        Region::ConstIterator end(region.constEnd());
        for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
            // insert substyle
            insert((*it)->rect(), subStyle);
            regionChanged((*it)->rect());
        }
    }
}

void StyleStorage::load(const QList<QPair<QRegion, Style> >& styles)
{
    QList<QPair<QRegion, SharedSubStyle> > subStyles;

    d->usedArea = QRegion();
    d->usedColumns.clear();
    d->usedRows.clear();
    d->cachedArea = QRegion();
    d->cache.clear();
    typedef QPair<QRegion, Style> StyleRegion;
    foreach (const StyleRegion& styleArea, styles) {
        const QRegion& reg = styleArea.first;
        const Style& style = styleArea.second;
        if (style.isEmpty()) continue;

        // update used areas
        foreach (const QRect& rect, reg.rects()) {
            if (rect.top() == 1 && rect.bottom() >= KS_rowMax) {
                for (int i = rect.left(); i <= rect.right(); ++i) {
                    d->usedColumns.insert(i, true);
                }
            } else if (rect.left() == 1 && rect.right() >= KS_colMax) {
                for (int i = rect.top(); i <= rect.bottom(); ++i) {
                    d->usedRows.insert(i, true);
                }
            } else {
                d->usedArea += rect;
            }
        }

        // find substyles
        foreach(const SharedSubStyle& subStyle, style.subStyles()) {
            bool foundShared = false;
            typedef const QList< SharedSubStyle> StoredSubStyleList;
            StoredSubStyleList& storedSubStyles(d->subStyles.value(subStyle->type()));
            StoredSubStyleList::ConstIterator end(storedSubStyles.end());
            for (StoredSubStyleList::ConstIterator it(storedSubStyles.begin()); it != end; ++it) {
                if (Style::compare(subStyle.data(), (*it).data())) {
        //             kDebug(36006) <<"[REUSING EXISTING SUBSTYLE]";
                    subStyles.append(qMakePair(reg, *it));
                    foundShared = true;
                    break;
                }
            }
            if (!foundShared) {
                // insert substyle and add to the used substyle list
                subStyles.append(qMakePair(reg, subStyle));
            }
        }
    }
    d->tree.load(subStyles);
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::insertRows(int position, int number)
{
    const QRect invalidRect(1, position, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // update the used area
    const QRegion usedArea = d->usedArea & invalidRect;
    d->usedArea -= invalidRect;
    d->usedArea += usedArea.translated(0, number);
    const QVector<QRect> rects = (d->usedArea & QRect(1, position - 1, KS_colMax, 1)).rects();
    for (int i = 0; i < rects.count(); ++i)
        d->usedArea += rects[i].adjusted(0, 1, 0, number + 1);
    // update the used rows
    QMap<int, bool> map;
    QMap<int, bool>::iterator begin = d->usedRows.upperBound(position);
    QMap<int, bool>::iterator end = d->usedRows.end();
    for (QMap<int, bool>::iterator it = begin; it != end; ++it) {
        if (it.key() + number <= KS_rowMax)
            map.insert(it.key() + number, true);
    }
    for (QMap<int, bool>::iterator it = begin; it != end; ++it)
        d->usedRows.remove(it.key());
    d->usedRows.unite(map);
    // process the tree
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(1, KS_rowMax - number + 1, KS_colMax, number), SharedSubStyle());
    undoData << d->tree.insertRows(position, number);
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::insertColumns(int position, int number)
{
    const QRect invalidRect(position, 1, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // update the used area
    const QRegion usedArea = d->usedArea & invalidRect;
    d->usedArea -= invalidRect;
    d->usedArea += usedArea.translated(number, 0);
    const QVector<QRect> rects = (d->usedArea & QRect(position - 1, 0, 1, KS_rowMax)).rects();
    for (int i = 0; i < rects.count(); ++i)
        d->usedArea += rects[i].adjusted(1, 0, number + 1, 0);
    // update the used columns
    QMap<int, bool> map;
    QMap<int, bool>::iterator begin = d->usedColumns.upperBound(position);
    QMap<int, bool>::iterator end = d->usedColumns.end();
    for (QMap<int, bool>::iterator it = begin; it != end; ++it) {
        if (it.key() + number <= KS_colMax)
            map.insert(it.key() + number, true);
    }
    for (QMap<int, bool>::iterator it = begin; it != end; ++it)
        d->usedColumns.remove(it.key());
    d->usedColumns.unite(map);
    // process the tree
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(KS_colMax - number + 1, 1, number, KS_rowMax), SharedSubStyle());
    undoData << d->tree.insertColumns(position, number);
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::removeRows(int position, int number)
{
    const QRect invalidRect(1, position, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // update the used area
    const QRegion usedArea = d->usedArea & QRect(1, position + number, KS_colMax, KS_rowMax);
    d->usedArea -= invalidRect;
    d->usedArea += usedArea.translated(0, -number);
    // update the used rows
    QMap<int, bool> map;
    QMap<int, bool>::iterator begin = d->usedRows.upperBound(position);
    QMap<int, bool>::iterator end = d->usedRows.end();
    for (QMap<int, bool>::iterator it = begin; it != end; ++it) {
        if (it.key() - number >= position)
            map.insert(it.key() - number, true);
    }
    for (QMap<int, bool>::iterator it = begin; it != end; ++it)
        d->usedRows.remove(it.key());
    d->usedRows.unite(map);
    // process the tree
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(1, position, KS_colMax, number), SharedSubStyle());
    undoData << d->tree.removeRows(position, number);
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::removeColumns(int position, int number)
{
    const QRect invalidRect(position, 1, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // update the used area
    const QRegion usedArea = d->usedArea & QRect(position + number, 1, KS_colMax, KS_rowMax);
    d->usedArea -= invalidRect;
    d->usedArea += usedArea.translated(-number, 0);
    // update the used columns
    QMap<int, bool> map;
    QMap<int, bool>::iterator begin = d->usedColumns.upperBound(position);
    QMap<int, bool>::iterator end = d->usedColumns.end();
    for (QMap<int, bool>::iterator it = begin; it != end; ++it) {
        if (it.key() - number >= position)
            map.insert(it.key() - number, true);
    }
    for (QMap<int, bool>::iterator it = begin; it != end; ++it)
        d->usedColumns.remove(it.key());
    d->usedColumns.unite(map);
    // process the tree
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(position, 1, number, KS_rowMax), SharedSubStyle());
    undoData << d->tree.removeColumns(position, number);
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::insertShiftRight(const QRect& rect)
{
    const QRect invalidRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom()));
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(rect), SharedSubStyle());
    undoData << d->tree.insertShiftRight(rect);
    regionChanged(invalidRect);
    // update the used area
    const QRegion usedArea = d->usedArea & invalidRect;
    d->usedArea -= invalidRect;
    d->usedArea += usedArea.translated(rect.width(), 0);
    const QVector<QRect> rects = (d->usedArea & QRect(rect.left() - 1, rect.top(), 1, rect.height())).rects();
    for (int i = 0; i < rects.count(); ++i)
        d->usedArea += rects[i].adjusted(1, 0, rect.width() + 1, 0);
    // update the used columns
    QMap<int, bool>::iterator begin = d->usedColumns.upperBound(rect.left());
    QMap<int, bool>::iterator end = d->usedColumns.end();
    for (QMap<int, bool>::iterator it = begin; it != end; ++it) {
        if (it.key() + rect.width() <= KS_colMax)
            d->usedArea += QRect(it.key() + rect.width(), rect.top(), rect.width(), rect.height());
    }
    if (d->usedColumns.contains(rect.left() - 1))
        d->usedArea += rect;
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::insertShiftDown(const QRect& rect)
{
    const QRect invalidRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax));
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(rect), SharedSubStyle());
    undoData << d->tree.insertShiftDown(rect);
    regionChanged(invalidRect);
    // update the used area
    const QRegion usedArea = d->usedArea & invalidRect;
    d->usedArea -= invalidRect;
    d->usedArea += usedArea.translated(0, rect.height());
    const QVector<QRect> rects = (d->usedArea & QRect(rect.left(), rect.top() - 1, rect.width(), 1)).rects();
    for (int i = 0; i < rects.count(); ++i)
        d->usedArea += rects[i].adjusted(0, 1, 0, rect.height() + 1);
    // update the used rows
    QMap<int, bool>::iterator begin = d->usedRows.upperBound(rect.top());
    QMap<int, bool>::iterator end = d->usedRows.end();
    for (QMap<int, bool>::iterator it = begin; it != end; ++it) {
        if (it.key() + rect.height() <= KS_rowMax)
            d->usedArea += QRect(rect.left(), it.key() + rect.height(), rect.width(), rect.height());
    }
    if (d->usedRows.contains(rect.top() - 1))
        d->usedArea += rect;
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::removeShiftLeft(const QRect& rect)
{
    const QRect invalidRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom()));
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(rect), SharedSubStyle());
    undoData << d->tree.removeShiftLeft(rect);
    regionChanged(invalidRect);
    // update the used area
    const QRegion usedArea = d->usedArea & QRect(rect.right() + 1, rect.top(), KS_colMax, rect.height());
    d->usedArea -= invalidRect;
    d->usedArea += usedArea.translated(-rect.width(), 0);
    // update the used columns
    QMap<int, bool>::iterator begin = d->usedColumns.upperBound(rect.right() + 1);
    QMap<int, bool>::iterator end = d->usedColumns.end();
    for (QMap<int, bool>::iterator it = begin; it != end; ++it) {
        if (it.key() - rect.width() >= rect.left())
            d->usedArea += QRect(it.key() - rect.width(), rect.top(), rect.width(), rect.height());
    }
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::removeShiftUp(const QRect& rect)
{
    const QRect invalidRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax));
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(rect), SharedSubStyle());
    undoData << d->tree.removeShiftUp(rect);
    regionChanged(invalidRect);
    // update the used area
    const QRegion usedArea = d->usedArea & QRect(rect.left(), rect.bottom() + 1, rect.width(), KS_rowMax);
    d->usedArea -= invalidRect;
    d->usedArea += usedArea.translated(0, -rect.height());
    // update the used rows
    QMap<int, bool>::iterator begin = d->usedRows.upperBound(rect.bottom() + 1);
    QMap<int, bool>::iterator end = d->usedRows.end();
    for (QMap<int, bool>::iterator it = begin; it != end; ++it) {
        if (it.key() - rect.height() >= rect.top())
            d->usedArea += QRect(rect.left(), it.key() - rect.height(), rect.width(), rect.height());
    }
    return undoData;
}

void StyleStorage::invalidateCache()
{
    d->cache.clear();
    d->cachedArea = QRegion();
}

void StyleStorage::garbageCollection()
{
    // any possible garbage left?
    if (d->possibleGarbage.isEmpty())
        return;

    const int currentZIndex = d->possibleGarbage.constBegin().key();
    const QPair<QRectF, SharedSubStyle> currentPair = d->possibleGarbage.take(currentZIndex);

    // check whether the named style still exists
    if (currentPair.second->type() == Style::NamedStyleKey &&
            !styleManager()->style(static_cast<const NamedStyle*>(currentPair.second.data())->name)) {
        kDebug(36006) << "removing" << currentPair.second->debugData()
        << "at" << Region(currentPair.first.toRect()).name()
        << "used" << currentPair.second->ref << "times" << endl;
        d->tree.remove(currentPair.first.toRect(), currentPair.second);
        d->subStyles[currentPair.second->type()].removeAll(currentPair.second);
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
        return; // already done
    }

    typedef QPair<QRectF, SharedSubStyle> SharedSubStylePair;
    QMap<int, SharedSubStylePair> pairs = d->tree.intersectingPairs(currentPair.first.toRect());
    if (pairs.isEmpty())   // actually never true, just for sanity
        return;
    int zIndex = pairs.constBegin().key();
    SharedSubStylePair pair = pairs[zIndex];

    // check whether the default style is placed first
    if (zIndex == currentZIndex &&
            currentPair.second->type() == Style::DefaultStyleKey &&
            pair.second->type() == Style::DefaultStyleKey &&
            pair.first == currentPair.first) {
        kDebug(36006) << "removing default style"
        << "at" << Region(currentPair.first.toRect()).name()
        << "used" << currentPair.second->ref << "times" << endl;
        d->tree.remove(currentPair.first.toRect(), currentPair.second);
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
        return; // already done
    }

    // special handling for indentation:
    // check whether the default indentation is placed first
    if (zIndex == currentZIndex &&
            currentPair.second->type() == Style::Indentation &&
            static_cast<const SubStyleOne<Style::Indentation, int>*>(currentPair.second.data())->value1 == 0 &&
            pair.first == currentPair.first) {
        kDebug(36006) << "removing default indentation"
        << "at" << Region(currentPair.first.toRect()).name()
        << "used" << currentPair.second->ref << "times" << endl;
        d->tree.remove(currentPair.first.toRect(), currentPair.second);
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
        return; // already done
    }

    // special handling for precision:
    // check whether the storage default precision is placed first
    if (zIndex == currentZIndex &&
            currentPair.second->type() == Style::Precision &&
            static_cast<const SubStyleOne<Style::Precision, int>*>(currentPair.second.data())->value1 == 0 &&
            pair.first == currentPair.first) {
        kDebug(36006) << "removing default precision"
        << "at" << Region(currentPair.first.toRect()).name()
        << "used" << currentPair.second->ref << "times" << endl;
        d->tree.remove(currentPair.first.toRect(), currentPair.second);
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
        return; // already done
    }

    // check, if the current substyle is covered by others added after it
    bool found = false;
    QMap<int, SharedSubStylePair>::ConstIterator end = pairs.constEnd();
    for (QMap<int, SharedSubStylePair>::ConstIterator it = pairs.constFind(currentZIndex); it != end; ++it) {
        zIndex = it.key();
        pair = it.value();

        // as long as the substyle in question was not found, skip the substyle
        if (!found) {
            if (pair.first == currentPair.first &&
                    Style::compare(pair.second.data(), currentPair.second.data()) &&
                    zIndex == currentZIndex) {
                found = true;
            }
            continue;
        }

        // remove the current pair, if another substyle of the same type,
        // the default style or a named style follows and the rectangle
        // is completely covered
        if (zIndex != currentZIndex &&
                (pair.second->type() == currentPair.second->type() ||
                 pair.second->type() == Style::DefaultStyleKey ||
                 pair.second->type() == Style::NamedStyleKey) &&
                pair.first.toRect().contains(currentPair.first.toRect())) {
            // special handling for indentation
            // only remove, if covered by default
            if (pair.second->type() == Style::Indentation &&
                    static_cast<const SubStyleOne<Style::Indentation, int>*>(pair.second.data())->value1 != 0) {
                continue;
            }

            // special handling for precision
            // only remove, if covered by default
            if (pair.second->type() == Style::Precision &&
                    static_cast<const SubStyleOne<Style::Precision, int>*>(pair.second.data())->value1 != 0) {
                continue;
            }

            kDebug(36006) << "removing" << currentPair.second->debugData()
            << "at" << Region(currentPair.first.toRect()).name()
            << "used" << currentPair.second->ref << "times" << endl;
            d->tree.remove(currentPair.first.toRect(), currentPair.second);
#if 0
            kDebug(36006) << "StyleStorage: usage of" << currentPair.second->debugData() << " is" << currentPair.second->ref;
            // FIXME Stefan: The usage of substyles used once should be
            //               two (?) here, not more. Why is this not the case?
            //               The shared pointers are used by:
            //               a) the tree
            //               b) the reusage list (where it should be removed)
            //               c) the cached styles (!)
            //               d) the undo data of operations (!)
            if (currentPair.second->ref == 2) {
                kDebug(36006) << "StyleStorage: removing" << currentPair.second << " from the used subStyles";
                d->subStyles[currentPair.second->type()].removeAll(currentPair.second);
            }
#endif
            break;
        }
    }
    QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
}

void StyleStorage::regionChanged(const QRect& rect)
{
    if (d->map->isLoading())
        return;
    // mark the possible garbage
    // NOTE Stefan: The map may contain multiple indices. The already existing possible garbage has
    // has to be inserted most recently, because it should be accessed first.
    d->possibleGarbage = d->tree.intersectingPairs(rect).unite(d->possibleGarbage);
    QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
    // invalidate cache
    invalidateCache(rect);
}

void StyleStorage::invalidateCache(const QRect& rect)
{
//     kDebug(36006) <<"StyleStorage: Invalidating" << rect;
    const QRegion region = d->cachedArea.intersected(rect);
    d->cachedArea = d->cachedArea.subtracted(rect);
    foreach(const QRect& rect, region.rects()) {
        for (int col = rect.left(); col <= rect.right(); ++col) {
            for (int row = rect.top(); row <= rect.bottom(); ++row) {
//                 kDebug(36006) <<"StyleStorage: Removing cached style for" << Cell::name( col, row );
                d->cache.remove(QPoint(col, row));     // also deletes it
            }
        }
    }
}

Style StyleStorage::composeStyle(const QList<SharedSubStyle>& subStyles) const
{
    if (subStyles.isEmpty())
        return *styleManager()->defaultStyle();

    Style style;
    for (int i = 0; i < subStyles.count(); ++i) {
        if (subStyles[i]->type() == Style::DefaultStyleKey)
            style = *styleManager()->defaultStyle();
        else if (subStyles[i]->type() == Style::NamedStyleKey) {
            style.clear();
            const CustomStyle* namedStyle = styleManager()->style(static_cast<const NamedStyle*>(subStyles[i].data())->name);
            if (namedStyle) {
                // first, load the attributes of the parent style(s)
                QList<CustomStyle*> parentStyles;
                CustomStyle* parentStyle = styleManager()->style(namedStyle->parentName());
//                 kDebug(36006) <<"StyleStorage:" << namedStyle->name() <<"'s parent =" << namedStyle->parentName();
                while (parentStyle) {
//                     kDebug(36006) <<"StyleStorage:" << parentStyle->name() <<"'s parent =" << parentStyle->parentName();
                    parentStyles.prepend(parentStyle);
                    parentStyle = styleManager()->style(parentStyle->parentName());
                }
                Style tmpStyle;
                for (int i = 0; i < parentStyles.count(); ++i) {
//                     kDebug(36006) <<"StyleStorage: merging" << parentStyles[i]->name() <<" in.";
                    tmpStyle = *parentStyles[i];
                    tmpStyle.merge(style);
                    style = tmpStyle;
                }
                // second, merge the other attributes in
//                 kDebug(36006) <<"StyleStorage: merging" << namedStyle->name() <<" in.";
                tmpStyle = *namedStyle;
                tmpStyle.merge(style);
                style = tmpStyle;
                // not the default anymore
                style.clearAttribute(Style::DefaultStyleKey);
                // reset the parent name
                style.setParentName(namedStyle->name());
//                 kDebug(36006) <<"StyleStorage: merging done";
            }
        } else if (subStyles[i]->type() == Style::Indentation) {
            // special handling for indentation
            const int indentation = static_cast<const SubStyleOne<Style::Indentation, int>*>(subStyles[i].data())->value1;
            if (indentation == 0 || (style.indentation() + indentation <= 0))
                style.clearAttribute(Style::Indentation);   // reset
            else
                style.setIndentation(style.indentation() + indentation);   // increase/decrease
        } else if (subStyles[i]->type() == Style::Precision) {
            // special handling for precision
            // The Style default (-1) and the storage default (0) differ.
            const int precision = static_cast<const SubStyleOne<Style::Precision, int>*>(subStyles[i].data())->value1;
            if (precision == 0)   // storage default
                style.clearAttribute(Style::Precision);   // reset
            else {
                if (style.precision() == -1)   // Style default
                    style.setPrecision(qMax(0, precision));     // positive initial value
                else if (style.precision() + precision <= 0)
                    style.setPrecision(0);
                else if (style.precision() + precision >= 10)
                    style.setPrecision(10);
                else
                    style.setPrecision(style.precision() + precision);   // increase/decrease
            }
        } else {
            // insert the substyle
//             kDebug(36006) <<"StyleStorage: inserting" << subStyles[i]->debugData();
            style.insertSubStyle(subStyles[i]);
            // not the default anymore
            style.clearAttribute(Style::DefaultStyleKey);
        }
    }
    return style;
}

StyleManager* StyleStorage::styleManager() const
{
    return d->map->styleManager();
}

#include "StyleStorage.moc"
