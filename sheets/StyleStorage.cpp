/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
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
#include <QRunnable>
#ifdef CALLIGRA_SHEETS_MT
#include <QMutex>
#include <QMutexLocker>
#endif

#include "Global.h"
#include "Map.h"
#include "RTree.h"
#include "Style.h"
#include "StyleManager.h"
#include "RectStorage.h"

static const int g_maximumCachedStyles = 10000;

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN StyleStorage::Private
{
public:
    Private()
#ifdef CALLIGRA_SHEETS_MT
        : cacheMutex(QMutex::Recursive)
#endif
    {}
    Map* map;
    RTree<SharedSubStyle> tree;
    QMap<int, bool> usedColumns; // FIXME Stefan: Use QList and std::upper_bound() for insertion.
    QMap<int, bool> usedRows;
    QRegion usedArea;
    QHash<Style::Key, QList<SharedSubStyle> > subStyles;
    QMap<int, QPair<QRectF, SharedSubStyle> > possibleGarbage;
    QCache<QPoint, Style> cache;
    QRegion cachedArea;
    StyleStorageLoaderJob* loader;
#ifdef CALLIGRA_SHEETS_MT
    QMutex cacheMutex;
#endif

    void ensureLoaded();
};

class Calligra::Sheets::StyleStorageLoaderJob : public QRunnable
{
public:
    StyleStorageLoaderJob(StyleStorage* storage, const QList<QPair<QRegion, Style> >& styles);
    void run() override;
    void waitForFinished();
    bool isFinished();
    QList<QPair<QRegion, Style> > data() const { return m_styles; }
private:
    StyleStorage* m_storage;
    QList<QPair<QRegion, Style> > m_styles;
};

StyleStorageLoaderJob::StyleStorageLoaderJob(StyleStorage *storage, const QList<QPair<QRegion, Style> > &styles)
    : m_storage(storage), m_styles(styles)
{

}

void StyleStorageLoaderJob::waitForFinished()
{
    run();
}

bool StyleStorageLoaderJob::isFinished()
{
    return false;
}

void StyleStorageLoaderJob::run()
{
    static int total = 0;
    debugSheetsStyle << "Loading styles:" << endl << m_styles;
    QTime t; t.start();
    StyleStorage::Private* d = m_storage->d;
    QList<QPair<QRegion, SharedSubStyle> > subStyles;

    d->usedArea = QRegion();
    d->usedColumns.clear();
    d->usedRows.clear();
    {
#ifdef CALLIGRA_SHEETS_MT
        QMutexLocker(&d->cacheMutex);
#endif
        d->cachedArea = QRegion();
        d->cache.clear();
    }
    typedef QPair<QRegion, Style> StyleRegion;
    foreach (const StyleRegion& styleArea, m_styles) {
        const QRegion& reg = styleArea.first;
        const Style& style = styleArea.second;
        if (style.isEmpty()) continue;

        // update used areas
        QRect bound = reg.boundingRect();
        if ((bound.top() == 1 && bound.bottom() >= KS_rowMax) || (bound.left() == 1 && bound.right() >= KS_colMax)) {
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
        } else {
            d->usedArea += reg;
        }

        // find substyles
        foreach(const SharedSubStyle& subStyle, style.subStyles()) {
            bool foundShared = false;
            typedef const QList< SharedSubStyle> StoredSubStyleList;
            StoredSubStyleList& storedSubStyles(d->subStyles.value(subStyle->type()));
            StoredSubStyleList::ConstIterator end(storedSubStyles.end());
            for (StoredSubStyleList::ConstIterator it(storedSubStyles.begin()); it != end; ++it) {
                if (Style::compare(subStyle.data(), (*it).data())) {
        //             debugSheetsStyle <<"[REUSING EXISTING SUBSTYLE]";
                    subStyles.append(qMakePair(reg, *it));
                    foundShared = true;
                    break;
                }
            }
            if (!foundShared) {
                // insert substyle and add to the used substyle list
                //if (reg.contains(QPoint(1,1))) {debugSheetsStyle<<"load:"<<reg<<':'; subStyle.data()->dump();}
                subStyles.append(qMakePair(reg, subStyle));
            }
        }
    }
    d->tree.load(subStyles);
    int e = t.elapsed();
    total += e;
    debugSheetsStyle << "Time: " << e << total;
}

void StyleStorage::Private::ensureLoaded()
{
    if (loader) {
        loader->waitForFinished();
        delete loader;
        loader = 0;
    }
}

StyleStorage::StyleStorage(Map* map)
        : QObject(map)
        , d(new Private)
{
    d->map = map;
    d->cache.setMaxCost(g_maximumCachedStyles);
    d->loader = 0;
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
    if (other.d->loader) {
        d->loader = new StyleStorageLoaderJob(this, other.d->loader->data());
    } else {
        d->loader = 0;
    }
    // the other member variables are temporary stuff
}

StyleStorage::~StyleStorage()
{
    delete d->loader; // in a multi-threaded approach this needs more care
    delete d;
}

Style StyleStorage::contains(const QPoint& point) const
{
    d->ensureLoaded();
    if (!d->usedArea.contains(point) && !d->usedColumns.contains(point.x()) && !d->usedRows.contains(point.y()))
        return *styleManager()->defaultStyle();

    {
#ifdef CALLIGRA_SHEETS_MT
        QMutexLocker ml(&d->cacheMutex);
#endif
        // first, lookup point in the cache
        if (d->cache.contains(point)) {
            Style st = *d->cache.object(point);
            //if (point.x() == 1 && point.y() == 1) {debugSheetsStyle <<"StyleStorage: cached style:"<<point<<':'; st.dump();}
            return st;
        }
    }
    // not found, lookup in the tree
    QList<SharedSubStyle> subStyles = d->tree.contains(point);
    //if (point.x() == 1 && point.y() == 1) {debugSheetsStyle <<"StyleStorage: substyles:"<<point<<':'; for (const SharedSubStyle &s : subStyles) {debugSheetsStyle<<s.data()->debugData();}}
    if (subStyles.isEmpty()) {
        Style *style = styleManager()->defaultStyle();
        // let's try caching empty styles too, the lookup is rather expensive still
        {
#ifdef CALLIGRA_SHEETS_MT
            QMutexLocker ml(&d->cacheMutex);
#endif
            // insert style into the cache
            d->cache.insert(point, style);
            d->cachedArea += QRect(point, point);
        }

        return *style;
    }
    Style* style = new Style();
    (*style) = composeStyle(subStyles);

    {
#ifdef CALLIGRA_SHEETS_MT
        QMutexLocker ml(&d->cacheMutex);
#endif
        // insert style into the cache
        d->cache.insert(point, style);
        d->cachedArea += QRect(point, point);
    }
    //if (point.x() == 1 && point.y() == 1) {debugSheetsStyle <<"StyleStorage: style:"<<point<<':'; style->dump();}
    return *style;
}

Style StyleStorage::contains(const QRect& rect) const
{
    d->ensureLoaded();
    QList<SharedSubStyle> subStyles = d->tree.contains(rect);
    return composeStyle(subStyles);
}

Style StyleStorage::intersects(const QRect& rect) const
{
    d->ensureLoaded();
    QList<SharedSubStyle> subStyles = d->tree.intersects(rect);
    return composeStyle(subStyles);
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::undoData(const Region& region) const
{
    d->ensureLoaded();
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
    d->ensureLoaded();
    if (d->usedArea.isEmpty())
        return QRect(1, 1, 0, 0);
    return QRect(QPoint(1, 1), d->usedArea.boundingRect().bottomRight());
}

// craete default styles in the style tables - used in Odf saving
void StyleStorage::saveCreateDefaultStyles(int& maxCols, int& maxRows, QMap<int, Style> &columnDefaultStyles, QMap<int, Style> &rowDefaultStyles) const
{
    d->ensureLoaded();
#if 0 // TODO
    // If we have both, column and row styles, we can take the short route.
    if (!d->usedColumns.isEmpty() && !d->usedRows.isEmpty()) {
        for (int i = 0; i < d->usedColumns.count(); ++i) {
            const int col = d->usedColumns[i];
            columnDefaultStyles[col].insertSubStyle(contains(QRect(col, 1, 1, KS_rowMax)));
        }
        for (int i = 0; i < d->usedRow.count(); ++i) {
            const int row = d->usedRow[i];
            rowDefaultStyles[row].insertSubStyle(contains(QRect(1, row, KS_colMax, 1)));
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
                    columnDefaultStyles.remove(col);
                else
                    columnDefaultStyles[col].insertSubStyle(pairs[i].second);
            }
        }
        // row default cell styles
        else if (rect.left() == 1 && rect.right() == maxCols) {
            for (int row = rect.top(); row <= rect.bottom(); ++row) {
                if (pairs[i].second.data()->type() == Style::DefaultStyleKey)
                    rowDefaultStyles.remove(row);
                else
                    rowDefaultStyles[row].insertSubStyle(pairs[i].second);
            }
        }
    }
}

int StyleStorage::nextColumnStyleIndex(int column) const
{
    d->ensureLoaded();
    QMap<int, bool>::iterator it = d->usedColumns.upperBound(column + 1);
    return (it == d->usedColumns.end()) ? 0 : it.key();
}

int StyleStorage::nextRowStyleIndex(int row) const
{
    d->ensureLoaded();
    QMap<int, bool>::iterator it = d->usedRows.upperBound(row + 1);
    return (it == d->usedRows.end()) ? 0 : it.key();
}

int StyleStorage::firstColumnIndexInRow(int row) const
{
    d->ensureLoaded();
    const QRect rect = (d->usedArea & QRect(QPoint(1, row), QPoint(KS_colMax, row))).boundingRect();
    return rect.isNull() ? 0 : rect.left();
}

int StyleStorage::nextColumnIndexInRow(int column, int row) const
{
    d->ensureLoaded();
    const QRect rect = (d->usedArea & QRect(QPoint(column + 1, row), QPoint(KS_colMax, row))).boundingRect();
    return rect.isNull() ? 0 : rect.left();
}

void StyleStorage::insert(const QRect& rect, const SharedSubStyle& subStyle, bool markRegionChanged)
{
    d->ensureLoaded();
//     debugSheetsStyle <<"StyleStorage: inserting" << SubStyle::name(subStyle->type()) <<" into" << rect;
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
//             debugSheetsStyle <<"[REUSING EXISTING SUBSTYLE]";
            d->tree.insert(rect, *it);
            if (markRegionChanged) {
                regionChanged(rect);
            }
            return;
        }
    }
    // insert substyle and add to the used substyle list
    d->tree.insert(rect, subStyle);
    d->subStyles[subStyle->type()].append(subStyle);
    if (markRegionChanged) {
        regionChanged(rect);
    }
}

void StyleStorage::insert(const Region& region, const Style& style)
{
    d->ensureLoaded();
    if (style.isEmpty())
        return;
    foreach(const SharedSubStyle& subStyle, style.subStyles()) {
        Region::ConstIterator end(region.constEnd());
        for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
            // insert substyle
            insert((*it)->rect(), subStyle, false);
        }
    }
    for (Region::ConstIterator it(region.constBegin()), end(region.constEnd()); it != end; ++it) {
        regionChanged((*it)->rect());
    }
}

void StyleStorage::load(const QList<QPair<QRegion, Style> >& styles)
{
    Q_ASSERT(!d->loader);
    d->loader = new StyleStorageLoaderJob(this, styles);
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::insertRows(int position, int number)
{
    d->ensureLoaded();
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
    QMap<int, bool>::iterator begin = d->usedRows.lowerBound(position);
    QMap<int, bool>::iterator end = d->usedRows.end();
    for (QMap<int, bool>::iterator it = begin; it != end; ++it) {
        if (it.key() + number <= KS_rowMax)
            map.insert(it.key() + number, true);
    }
    for (QMap<int, bool>::iterator it = begin; it != d->usedRows.end(); )
        it = d->usedRows.erase(it);
    d->usedRows.unite(map);
    // process the tree
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(1, KS_rowMax - number + 1, KS_colMax, number), SharedSubStyle());
    undoData << d->tree.insertRows(position, number);
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::insertColumns(int position, int number)
{
    d->ensureLoaded();
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
    for (QMap<int, bool>::iterator it = begin; it != d->usedColumns.end(); )
        it = d->usedColumns.erase(it);
    d->usedColumns.unite(map);
    // process the tree
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(KS_colMax - number + 1, 1, number, KS_rowMax), SharedSubStyle());
    undoData << d->tree.insertColumns(position, number);
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::removeRows(int position, int number)
{
    d->ensureLoaded();
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
    for (QMap<int, bool>::iterator it = begin; it != d->usedRows.end(); )
        it = d->usedRows.erase(it);
    d->usedRows.unite(map);
    // process the tree
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(1, position, KS_colMax, number), SharedSubStyle());
    undoData << d->tree.removeRows(position, number);
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::removeColumns(int position, int number)
{
    d->ensureLoaded();
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
    for (QMap<int, bool>::iterator it = begin; it != d->usedColumns.end(); )
        it = d->usedColumns.erase(it);
    d->usedColumns.unite(map);
    // process the tree
    QList< QPair<QRectF, SharedSubStyle> > undoData;
    undoData << qMakePair(QRectF(position, 1, number, KS_rowMax), SharedSubStyle());
    undoData << d->tree.removeColumns(position, number);
    return undoData;
}

QList< QPair<QRectF, SharedSubStyle> > StyleStorage::insertShiftRight(const QRect& rect)
{
    d->ensureLoaded();
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
    d->ensureLoaded();
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
    d->ensureLoaded();
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
    d->ensureLoaded();
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
    // still busy loading? no cache to invalidate
    if (d->loader && !d->loader->isFinished())
        return;

#ifdef CALLIGRA_SHEETS_MT
    QMutexLocker ml(&d->cacheMutex);
#endif
    d->cache.clear();
    d->cachedArea = QRegion();
}

void StyleStorage::garbageCollection()
{
    // still busy loading? no garbage to collect
    if (d->loader && !d->loader->isFinished())
        return;

    // any possible garbage left?
    if (d->possibleGarbage.isEmpty())
        return;

    const int currentZIndex = d->possibleGarbage.constBegin().key();
    const QPair<QRectF, SharedSubStyle> currentPair = d->possibleGarbage.take(currentZIndex);

    // check whether the named style still exists
    if (currentPair.second->type() == Style::NamedStyleKey &&
            !styleManager()->style(static_cast<const NamedStyle*>(currentPair.second.data())->name)) {
        debugSheetsStyle << "removing" << currentPair.second->debugData()
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
        debugSheetsStyle << "removing default style"
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
        debugSheetsStyle << "removing default indentation"
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
        debugSheetsStyle << "removing default precision"
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

            debugSheetsStyle << "removing" << currentPair.second->debugData()
            << "at" << Region(currentPair.first.toRect()).name()
            << "used" << currentPair.second->ref << "times" << endl;
            d->tree.remove(currentPair.first.toRect(), currentPair.second, currentZIndex);
#if 0
            debugSheetsStyle << "StyleStorage: usage of" << currentPair.second->debugData() << " is" << currentPair.second->ref;
            // FIXME Stefan: The usage of substyles used once should be
            //               two (?) here, not more. Why is this not the case?
            //               The shared pointers are used by:
            //               a) the tree
            //               b) the reusage list (where it should be removed)
            //               c) the cached styles (!)
            //               d) the undo data of operations (!)
            if (currentPair.second->ref == 2) {
                debugSheetsStyle << "StyleStorage: removing" << currentPair.second << " from the used subStyles";
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
    // still busy loading? no garbage to collect
    if (d->loader && !d->loader->isFinished())
        return;
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
    // still busy loading? no cache to invalidate
    if (d->loader && !d->loader->isFinished())
        return;

#ifdef CALLIGRA_SHEETS_MT
    QMutexLocker ml(&d->cacheMutex);
#endif
//     debugSheetsStyle <<"StyleStorage: Invalidating" << rect;
    const QRegion region = d->cachedArea.intersected(rect);
    d->cachedArea = d->cachedArea.subtracted(rect);
    foreach(const QRect& rect, region.rects()) {
        for (int col = rect.left(); col <= rect.right(); ++col) {
            for (int row = rect.top(); row <= rect.bottom(); ++row) {
//                 debugSheetsStyle <<"StyleStorage: Removing cached style for" << Cell::name( col, row );
                d->cache.remove(QPoint(col, row));     // also deletes it
            }
        }
    }
}

Style StyleStorage::composeStyle(const QList<SharedSubStyle>& subStyles) const
{
    d->ensureLoaded();

    if (subStyles.isEmpty()) {
//         debugSheetsStyle <<"StyleStorage:" << "nothing to merge, return the default style";
        return *styleManager()->defaultStyle();
    }
    // From OpenDocument-v1.2-os-part1 16.2<style:style>
    //
    // The <style:style> element represents styles.
    //
    // Styles defined by the <style:style> element use a hierarchical style model.
    // The <style:style> element supports inheritance of formatting properties by a style from its parent style.
    // A parent style is specified by the style:parent-style-name attribute on a <style:style> element.
    //
    // The determination of the value of a formatting property begins with any style that is specified by an element.
    // If the formatting property is present in that style, its value is used.
    //
    // If that style does not specify a value for that formatting property and it has a parent style,
    // the value of the formatting element is taken from the parent style, if present.
    //
    // If the parent style does not have a value for the formatting property, the search for the formatting property value continues up parent styles
    // until either the formatting property has been found or a style is found with no parent style.
    //
    // If a search of the parent styles of a style does not result in a value for a formatting property,
    // the determination of its value depends on the style family and the element to which a style is applied.

    // TODO review loading of libreOffice generated files:
    // It seems libreOffice saves parent also when parent is the Default style.
    // Sheets do not do this, it is handled implicitly.
    // According to the spec, both ways should be ok,
    // but the result is that when loading lo files, it may exist multiple (two) NamedStyleKey substyles:
    // One loaded explicitly (first in the list), and one generated by our loading code (later in the list).
    // We use the last one in the list here, this should be our generated one.
    CustomStyle *namedStyle = 0;
    for (int i = subStyles.count() - 1; i >= 0; --i) {
        if (subStyles[i]->type() == Style::NamedStyleKey) {
            namedStyle = styleManager()->style(static_cast<const NamedStyle*>(subStyles[i].data())->name);
            if (namedStyle) {
                debugSheetsStyle<<"Compose found namedstyle:"<<static_cast<const NamedStyle*>(subStyles[i].data())->name<<namedStyle->parentName();namedStyle->dump();
                break;
            }
        }
    }

    Style style;
    // get attributes from parent styles
    if (namedStyle) {
        // first, load the attributes of the parent style(s)
        QList<CustomStyle*> parentStyles;
        CustomStyle *parentStyle = styleManager()->style(namedStyle->parentName());
        // debugSheetsStyle <<"StyleStorage:" << namedStyle->name() <<"'s parent =" << namedStyle->parentName();
        while (parentStyle) {
            // debugSheetsStyle <<"StyleStorage:" << parentStyle->name() <<"'s parent =" << parentStyle->parentName();
            parentStyles.prepend(parentStyle);
            parentStyle = styleManager()->style(parentStyle->parentName());
        }
        Style tmpStyle;
        for (int i = 0; i < parentStyles.count(); ++i) {
            // debugSheetsStyle <<"StyleStorage: merging" << parentStyles[i]->name() <<" in.";
            tmpStyle = *parentStyles[i];
            tmpStyle.merge(style); // insert/replace substyles in tmpStyle with substyles from style
            style = tmpStyle;
        }
        // second, merge the other attributes in
        // debugSheetsStyle <<"StyleStorage: merging" << namedStyle->name() <<" in.";
        tmpStyle = *namedStyle;
        tmpStyle.merge(style); // insert/replace substyles in tmpStyle with substyles from style
        style = tmpStyle;
        // not the default anymore
        style.clearAttribute(Style::DefaultStyleKey);
        // reset the parent name
        style.setParentName(namedStyle->name());
        //                 debugSheetsStyle <<"StyleStorage: merging done";
    }
    for (int i = 0; i < subStyles.count(); ++i) {
        if (subStyles[i]->type() == Style::DefaultStyleKey) {
            // skip
        } else if (subStyles[i]->type() == Style::NamedStyleKey) {
            // treated above
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
//             debugSheetsStyle <<"StyleStorage: inserting" << subStyles[i]->debugData();
            style.insertSubStyle(subStyles[i]);
            // not the default anymore
            style.clearAttribute(Style::DefaultStyleKey);
        }
    }
    // Implicitly merge in any missing attributes from the family (table-cell) default style
    // It might have been merged in via parent styles above, but we cannot rely on that.
    if (!styleManager()->defaultStyle()->isEmpty()) {
        // debugSheetsStyle << "StyleStorage: merging family default in";
        Style tmpStyle = *styleManager()->defaultStyle();
        tmpStyle.clearAttribute(Style::DefaultStyleKey);
        tmpStyle.merge(style); // insert/replace substyles in tmpStyle with substyles from style
        style = tmpStyle;
    }
    return style;
}

StyleManager* StyleStorage::styleManager() const
{
    return d->map->styleManager();
}
