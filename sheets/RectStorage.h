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

#ifndef CALLIGRA_SHEETS_RECT_STORAGE
#define CALLIGRA_SHEETS_RECT_STORAGE

#include <QCache>
#include <QRegion>
#include <QTimer>
#include <QRunnable>
#include <QTime>
#ifdef CALLIGRA_SHEETS_MT
#include <QMutex>
#include <QMutexLocker>
#endif

#include "sheets_odf_export.h"

#include "Map.h"
#include "Region.h"
#include "RTree.h"

static const int g_garbageCollectionTimeOut = 100;

namespace Calligra
{
namespace Sheets
{

template<typename T>
class RectStorageLoader;

/**
 * \ingroup Storage
 * A custom rectangular storage.
 * Based on an R-Tree data structure.
 * Usable for any kind of data attached to rectangular regions.
 *
 * Acts mainly as a wrapper around the R-Tree data structure to allow a future
 * replacement of this backend. Decorated with some additional features like
 * garbage collection, caching, used area tracking, etc.
 *
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 *
 * \note For data assigned to points use PointStorage.
 */
template<typename T>
class RectStorage
{
public:
    explicit RectStorage(Map* map);
    RectStorage(const RectStorage& other);
    virtual ~RectStorage();

    /**
     * \return the stored value at the position \p point .
     */
    T contains(const QPoint& point) const;

    /**
     * \return the stored rect/value pair at the position \p point .
     */
    QPair<QRectF, T> containedPair(const QPoint& point) const;

    QList< QPair<QRectF, T> > intersectingPairs(const Region& region) const;

    QList< QPair<QRectF, T> > undoData(const Region& region) const;

    /**
     * Returns the area, which got data attached.
     * \return the area using data
     */
    QRect usedArea() const;

    /**
     * Mass loading of data, removes any existing data first
     */
    void load(const QList<QPair<QRegion, T> >& data);

    /**
     * Assigns \p data to \p region .
     */
    void insert(const Region& region, const T& data);

    /**
     * Removes \p data from \p region .
     */
    void remove(const Region& region, const T& data);

    /**
     * Inserts \p number rows at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, T> > insertRows(int position, int number);

    /**
     * Inserts \p number columns at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, T> > insertColumns(int position, int number);

    /**
     * Deletes \p number rows at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, T> > removeRows(int position, int number);

    /**
     * Deletes \p number columns at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, T> > removeColumns(int position, int number);

    /**
     * Shifts the rows right of \p rect to the right by the width of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, T> > insertShiftRight(const QRect& rect);

    /**
     * Shifts the columns at the bottom of \p rect to the bottom by the height of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    QList< QPair<QRectF, T> > insertShiftDown(const QRect& rect);

    /**
     * Shifts the rows left of \p rect to the left by the width of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    QList< QPair<QRectF, T> > removeShiftLeft(const QRect& rect);

    /**
     * Shifts the columns on top of \p rect to the top by the height of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    QList< QPair<QRectF, T> > removeShiftUp(const QRect& rect);

protected:
    virtual void triggerGarbageCollection();
    virtual void garbageCollection();

    /**
     * Triggers all necessary actions after a change of \p rect .
     * Calls invalidateCache() and adds the data in
     * \p rect to the list of possible garbage.
     */
    void regionChanged(const QRect& rect);

    /**
     * Invalidates all cached styles lying in \p rect .
     */
    void invalidateCache(const QRect& rect);

    /**
     * Ensures that any load() operation has completed.
     */
    void ensureLoaded() const;
private:
    Map* m_map;
    RTree<T> m_tree;
    QRegion m_usedArea;
    QMap<int, QPair<QRectF, T> > m_possibleGarbage;
    QList<T> m_storedData;
    mutable QCache<QPoint, T> m_cache;
#ifdef CALLIGRA_SHEETS_MT
    mutable QMutex m_mutex;
#endif
    mutable QRegion m_cachedArea;

    RectStorageLoader<T>* m_loader;
    friend class RectStorageLoader<T>;
};

template<typename T>
class RectStorageLoader : public QRunnable
{
public:
    RectStorageLoader(RectStorage<T>* storage, const QList<QPair<QRegion, T> >& data);
    void run() override;
    void waitForFinished();
    bool isFinished() const;
    QList<QPair<QRegion, T> > data() const;
private:
    RectStorage<T>* m_storage;
    QList<QPair<QRegion, T> > m_data;
};

template<typename T>
RectStorage<T>::RectStorage(Map* map)
        : m_map(map), m_loader(0)
{
}

template<typename T>
RectStorage<T>::RectStorage(const RectStorage& other)
        : m_map(other.m_map)
        , m_usedArea(other.m_usedArea)
        , m_storedData(other.m_storedData)
        , m_loader(0)
{
    m_tree = other.m_tree;
    if (other.m_loader) {
        m_loader = new RectStorageLoader<T>(this, other.m_loader->data());
    }
}

template<typename T>
RectStorage<T>::~RectStorage()
{
    delete m_loader; // needs fixing if this ever gets to be multithreaded
}

template<typename T>
T RectStorage<T>::contains(const QPoint& point) const
{
    ensureLoaded();
#ifdef CALLIGRA_SHEETS_MT
    QMutexLocker ml(&m_mutex);
#endif
    if (!usedArea().contains(point))
        return T();
    // first, lookup point in the cache
    if (m_cache.contains(point)) {
        return *m_cache.object(point);
    }
    // not found, lookup in the tree
    QList<T> results = m_tree.contains(point);
    T data = results.isEmpty() ? T() : results.last();
    // insert style into the cache
    m_cache.insert(point, new T(data));
    m_cachedArea += QRect(point, point);
    return data;
}

template<typename T>
QPair<QRectF, T> RectStorage<T>::containedPair(const QPoint& point) const
{
    ensureLoaded();
    const QList< QPair<QRectF, T> > results = m_tree.intersectingPairs(QRect(point, point)).values();
    return results.isEmpty() ? qMakePair(QRectF(), T()) : results.last();
}

template<typename T>
QList< QPair<QRectF, T> > RectStorage<T>::intersectingPairs(const Region& region) const
{
    ensureLoaded();
    QList< QPair<QRectF, T> > result;
    Region::ConstIterator end = region.constEnd();
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it)
        result += m_tree.intersectingPairs((*it)->rect()).values();
    return result;
}

template<typename T>
QList< QPair<QRectF, T> > RectStorage<T>::undoData(const Region& region) const
{
    ensureLoaded();
    QList< QPair<QRectF, T> > result;
    Region::ConstIterator end = region.constEnd();
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
        const QRect rect = (*it)->rect();
        QList< QPair<QRectF, T> > pairs = m_tree.intersectingPairs(rect).values();
        for (int i = 0; i < pairs.count(); ++i) {
            // trim the rects
            pairs[i].first = pairs[i].first.intersected(rect);
        }
        // Always add a default value even if there are no pairs.
        result << qMakePair(QRectF(rect), T()) << pairs;
    }
    return result;
}

template<typename T>
QRect RectStorage<T>::usedArea() const
{
    ensureLoaded();
    return m_tree.boundingBox().toRect();
}

template<typename T>
void RectStorage<T>::load(const QList<QPair<QRegion, T> >& data)
{
    Q_ASSERT(!m_loader);
    m_loader = new RectStorageLoader<T>(this, data);
}

template<typename T>
void RectStorage<T>::insert(const Region& region, const T& _data)
{
    ensureLoaded();
    T data;
    // lookup already used data
    int index = m_storedData.indexOf(_data);
    if (index != -1)
        data = m_storedData[index];
    else {
        data = _data;
        m_storedData.append(_data);
    }

    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        // insert data
        m_tree.insert((*it)->rect(), data);
        regionChanged((*it)->rect());
    }
}

template<typename T>
void RectStorage<T>::remove(const Region& region, const T& data)
{
    ensureLoaded();
    if (!m_storedData.contains(data)) {
        return;
    }
    const Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        // remove data
        m_tree.remove((*it)->rect(), data);
        regionChanged((*it)->rect());
    }
}

template<typename T>
QList< QPair<QRectF, T> > RectStorage<T>::insertRows(int position, int number)
{
    ensureLoaded();
    const QRect invalidRect(1, position, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // process the tree
    QList< QPair<QRectF, T> > undoData;
    undoData << qMakePair(QRectF(1, KS_rowMax - number + 1, KS_colMax, number), T());
    undoData << m_tree.insertRows(position, number, RTree<T>::CopyCurrent);
    return undoData;
}

template<typename T>
QList< QPair<QRectF, T> > RectStorage<T>::insertColumns(int position, int number)
{
    ensureLoaded();
    const QRect invalidRect(position, 1, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // process the tree
    QList< QPair<QRectF, T> > undoData;
    undoData << qMakePair(QRectF(KS_colMax - number + 1, 1, number, KS_rowMax), T());
    undoData << m_tree.insertColumns(position, number, RTree<T>::CopyCurrent);
    return undoData;
}

template<typename T>
QList< QPair<QRectF, T> > RectStorage<T>::removeRows(int position, int number)
{
    ensureLoaded();
    const QRect invalidRect(1, position, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // process the tree
    QList< QPair<QRectF, T> > undoData;
    undoData << qMakePair(QRectF(1, position, KS_colMax, number), T());
    undoData << m_tree.removeRows(position, number);
    return undoData;
}

template<typename T>
QList< QPair<QRectF, T> > RectStorage<T>::removeColumns(int position, int number)
{
    ensureLoaded();
    const QRect invalidRect(position, 1, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // process the tree
    QList< QPair<QRectF, T> > undoData;
    undoData << qMakePair(QRectF(position, 1, number, KS_rowMax), T());
    undoData << m_tree.removeColumns(position, number);
    return undoData;
}

template<typename T>
QList< QPair<QRectF, T> > RectStorage<T>::insertShiftRight(const QRect& rect)
{
    ensureLoaded();
    const QRect invalidRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom()));
    QList< QPair<QRectF, T> > undoData;
    undoData << qMakePair(QRectF(rect), T());
    undoData << m_tree.insertShiftRight(rect);
    regionChanged(invalidRect);
    return undoData;
}

template<typename T>
QList< QPair<QRectF, T> > RectStorage<T>::insertShiftDown(const QRect& rect)
{
    ensureLoaded();
    const QRect invalidRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax));
    QList< QPair<QRectF, T> > undoData;
    undoData << qMakePair(QRectF(rect), T());
    undoData << m_tree.insertShiftDown(rect);
    regionChanged(invalidRect);
    return undoData;
}

template<typename T>
QList< QPair<QRectF, T> > RectStorage<T>::removeShiftLeft(const QRect& rect)
{
    ensureLoaded();
    const QRect invalidRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom()));
    QList< QPair<QRectF, T> > undoData;
    undoData << qMakePair(QRectF(rect), T());
    undoData << m_tree.removeShiftLeft(rect);
    regionChanged(invalidRect);
    return undoData;
}

template<typename T>
QList< QPair<QRectF, T> > RectStorage<T>::removeShiftUp(const QRect& rect)
{
    ensureLoaded();
    const QRect invalidRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax));
    QList< QPair<QRectF, T> > undoData;
    undoData << qMakePair(QRectF(rect), T());
    undoData << m_tree.removeShiftUp(rect);
    regionChanged(invalidRect);
    return undoData;
}

template<typename T>
void RectStorage<T>::triggerGarbageCollection()
{
}

template<typename T>
void RectStorage<T>::garbageCollection()
{
    if (m_loader && !m_loader->isFinished())
        return;

    // any possible garbage left?
    if (m_possibleGarbage.isEmpty())
        return;

    const int currentZIndex = m_possibleGarbage.constBegin().key();
    const QPair<QRectF, T> currentPair = m_possibleGarbage.take(currentZIndex);

    typedef QPair<QRectF, T> DataPair;
    QMap<int, DataPair> pairs = m_tree.intersectingPairs(currentPair.first.toRect());
    if (pairs.isEmpty())   // actually never true, just for sanity
        return;
    int zIndex = pairs.constBegin().key();
    DataPair pair = pairs[zIndex];

    // check whether the default style is placed first
    if (zIndex == currentZIndex &&
            currentPair.second == T() &&
            pair.second == T() &&
            pair.first == currentPair.first) {
        debugSheets << "RectStorage: removing default data at" << Region(currentPair.first.toRect()).name();
        m_tree.remove(currentPair.first.toRect(), currentPair.second);
        triggerGarbageCollection();
        return; // already done
    }

    bool found = false;
    typename QMap<int, DataPair>::ConstIterator end = pairs.constEnd();
    for (typename QMap<int, DataPair>::ConstIterator it = pairs.constFind(currentZIndex); it != end; ++it) {
        zIndex = it.key();
        pair = it.value();

        // as long as the substyle in question was not found, skip the substyle
        if (!found) {
            if (zIndex == currentZIndex &&
                    pair.first == currentPair.first &&
                    pair.second == currentPair.second) {
                found = true;
            }
            continue;
        }

        // remove the current pair, if another substyle of the same type,
        // the default style or a named style follows and the rectangle
        // is completely covered
        if (zIndex != currentZIndex &&
                (pair.second == currentPair.second || pair.second == T()) &&
                pair.first.toRect().contains(currentPair.first.toRect())) {
            debugSheets << "RectStorage: removing data at" << Region(currentPair.first.toRect()).name();
            m_tree.remove(currentPair.first.toRect(), currentPair.second);
            break;
        }
    }
    triggerGarbageCollection();
}

template<typename T>
void RectStorage<T>::regionChanged(const QRect& rect)
{
    if (m_loader && !m_loader->isFinished())
        return;
    if (m_map->isLoading())
        return;
    // mark the possible garbage
    // NOTE Stefan: The map may contain multiple indices. The already existing possible garbage has
    // has to be inserted most recently, because it should be accessed first.
    m_possibleGarbage = m_tree.intersectingPairs(rect).unite(m_possibleGarbage);
    triggerGarbageCollection();
    // invalidate cache
    invalidateCache(rect);
}

template<typename T>
void RectStorage<T>::invalidateCache(const QRect& invRect)
{
    if (m_loader && !m_loader->isFinished())
        return;
#ifdef CALLIGRA_SHEETS_MT
    QMutexLocker ml(&m_mutex);
#endif
    const QVector<QRect> rects = m_cachedArea.intersected(invRect).rects();
    m_cachedArea = m_cachedArea.subtracted(invRect);
    foreach(const QRect& rect, rects) {
        for (int col = rect.left(); col <= rect.right(); ++col) {
            for (int row = rect.top(); row <= rect.bottom(); ++row)
                m_cache.remove(QPoint(col, row));     // also deletes it
        }
    }
}

template<typename T>
void RectStorage<T>::ensureLoaded() const
{
    if (m_loader) {
        m_loader->waitForFinished();
        delete m_loader;
        const_cast<RectStorage<T>*>(this)->m_loader = 0;
    }
}

template<typename T>
RectStorageLoader<T>::RectStorageLoader(RectStorage<T> *storage, const QList<QPair<QRegion, T> > &data)
    : m_storage(storage)
    , m_data(data)
{
}

template<typename T>
void RectStorageLoader<T>::run()
{
    static int total = 0;
    debugSheets << "Loading conditional styles";
    QTime t; t.start();

    QList<QPair<QRegion, T> > treeData;
    typedef QPair<QRegion, T> TRegion;
    foreach (const TRegion& tr, m_data) {
        const QRegion& reg = tr.first;
        const T& d = tr.second;

        int index = m_storage->m_storedData.indexOf(d);
        if (index != -1) {
            treeData.append(qMakePair(reg, m_storage->m_storedData[index]));
        } else {
            treeData.append(tr);
            m_storage->m_storedData.append(d);
        }
    }

    m_storage->m_tree.load(treeData);
    int e = t.elapsed();
    total += e;
    debugSheets << "Time: " << e << total;
}

template<typename T>
void RectStorageLoader<T>::waitForFinished()
{
    run();
}

template<typename T>
bool RectStorageLoader<T>::isFinished() const
{
    return false;
}

template<typename T>
QList<QPair<QRegion, T> > RectStorageLoader<T>::data() const
{
     return m_data;
}

class CommentStorage : public QObject, public RectStorage<QString>
{
    Q_OBJECT
public:
    explicit CommentStorage(Map* map) : QObject(map), RectStorage<QString>(map) {}
    CommentStorage(const CommentStorage& other) : QObject(other.parent()), RectStorage<QString>(other) {}

protected Q_SLOTS:
    void triggerGarbageCollection() override {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
    }
    void garbageCollection() override {
        RectStorage<QString>::garbageCollection();
    }
};



class CALLIGRA_SHEETS_ODF_EXPORT FusionStorage : public QObject, public RectStorage<bool>
{
    Q_OBJECT
public:
    explicit FusionStorage(Map* map) : QObject(map), RectStorage<bool>(map) {}
    FusionStorage(const FusionStorage& other) : QObject(other.parent()), RectStorage<bool>(other) {}

protected Q_SLOTS:
    void triggerGarbageCollection() override {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
    }
    void garbageCollection() override {
        RectStorage<bool>::garbageCollection();
    }
};



class MatrixStorage : public QObject, public RectStorage<bool>
{
    Q_OBJECT
public:
    explicit MatrixStorage(Map* map) : QObject(map), RectStorage<bool>(map) {}
    MatrixStorage(const MatrixStorage& other) : QObject(other.parent()), RectStorage<bool>(other) {}

protected Q_SLOTS:
    void triggerGarbageCollection() override {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
    }
    void garbageCollection() override {
        RectStorage<bool>::garbageCollection();
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_RECT_STORAGE
