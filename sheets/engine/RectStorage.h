/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2006, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_RECT_STORAGE
#define CALLIGRA_SHEETS_RECT_STORAGE

#include <QCache>
#include <QElapsedTimer>
#include <QRunnable>
#include <QString>
#include <QTimer>
#ifdef CALLIGRA_SHEETS_MT
#include <QMutex>
#include <QMutexLocker>
#endif

#include "sheets_engine_export.h"

#include "MapBase.h"
#include "RTree.h"
#include "Region.h"
#include "StorageBase.h"

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
class RectStorage : public StorageBase
{
public:
    explicit RectStorage(MapBase *map);
    RectStorage(const RectStorage &other);
    virtual ~RectStorage();

    /**
     * \return the stored value at the position \p point .
     */
    T contains(const QPoint &point) const;

    /**
     * \return the stored rect/value pair at the position \p point .
     */
    QPair<QRectF, T> containedPair(const QPoint &point) const;

    QVector<QPair<QRectF, T>> intersectingPairs(const Region &region) const;

    QVector<QPair<QRectF, T>> currentData(const Region &region) const;

    /**
     * Returns the area, which got data attached.
     * \return the area using data
     */
    QRect usedArea() const;

    /**
     * Mass loading of data, removes any existing data first
     */
    void load(const QList<QPair<Region, T>> &data);

    /**
     * Assigns \p data to \p region .
     */
    void insert(const Region &region, const T &data);

    /**
     * Removes \p data from \p region .
     */
    void remove(const Region &region, const T &data);

    /**
     * Inserts \p number rows at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    void insertRows(int position, int number) override;

    /**
     * Inserts \p number columns at the position \p position .
     * It extends or shifts rectangles, respectively.
     */
    void insertColumns(int position, int number) override;

    /**
     * Deletes \p number rows at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    void removeRows(int position, int number) override;

    /**
     * Deletes \p number columns at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     */
    void removeColumns(int position, int number) override;

    /**
     * Shifts the rows right of \p rect to the right by the width of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    void insertShiftRight(const QRect &rect) override;

    /**
     * Shifts the columns at the bottom of \p rect to the bottom by the height of \p rect .
     * It extends or shifts rectangles, respectively.
     */
    void insertShiftDown(const QRect &rect) override;

    /**
     * Shifts the rows left of \p rect to the left by the width of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    void removeShiftLeft(const QRect &rect) override;

    /**
     * Shifts the columns on top of \p rect to the top by the height of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    void removeShiftUp(const QRect &rect) override;

    const QVector<QPair<QRectF, T>> &undoData() const;

    void resetUndo() override;

protected:
    virtual void triggerGarbageCollection();
    virtual void garbageCollection();

    /**
     * Triggers all necessary actions after a change of \p rect .
     * Calls invalidateCache() and adds the data in
     * \p rect to the list of possible garbage.
     */
    void regionChanged(const QRect &rect);

    /**
     * Invalidates all cached styles lying in \p rect .
     */
    void invalidateCache(const QRect &rect);

    /**
     * Ensures that any load() operation has completed.
     */
    void ensureLoaded() const;

private:
    MapBase *m_map;
    RTree<T> m_tree;
    Region m_usedArea;
    QMultiMap<int, QPair<QRectF, T>> m_possibleGarbage;
    mutable QCache<QPoint, T> m_cache;
#ifdef CALLIGRA_SHEETS_MT
    mutable QMutex m_mutex;
#endif
    mutable Region m_cachedArea;

    QVector<QPair<QRectF, T>> m_undoData;

    RectStorageLoader<T> *m_loader;
    friend class RectStorageLoader<T>;
};

template<typename T>
class RectStorageLoader : public QRunnable
{
public:
    RectStorageLoader(RectStorage<T> *storage, const QList<QPair<Region, T>> &data);
    void run() override;
    void waitForFinished();
    bool isFinished() const;
    QList<QPair<Region, T>> data() const;

private:
    RectStorage<T> *m_storage;
    QList<QPair<Region, T>> m_data;
};

template<typename T>
RectStorage<T>::RectStorage(MapBase *map)
    : m_map(map)
    , m_loader(nullptr)
{
}

template<typename T>
RectStorage<T>::RectStorage(const RectStorage &other)
    : m_map(other.m_map)
    , m_usedArea(other.m_usedArea)
    , m_loader(nullptr)
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
T RectStorage<T>::contains(const QPoint &point) const
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
    m_cachedArea.add(point);
    return data;
}

template<typename T>
QPair<QRectF, T> RectStorage<T>::containedPair(const QPoint &point) const
{
    ensureLoaded();
    const QVector<QPair<QRectF, T>> results = m_tree.intersectingPairs(QRect(point, point)).values().toVector();
    return results.isEmpty() ? qMakePair(QRectF(), T()) : results.last();
}

template<typename T>
QVector<QPair<QRectF, T>> RectStorage<T>::intersectingPairs(const Region &region) const
{
    ensureLoaded();
    QVector<QPair<QRectF, T>> result;
    Region::ConstIterator end = region.constEnd();
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it)
        result.append(m_tree.intersectingPairs((*it)->rect()).values().toVector());
    return result;
}

template<typename T>
QRect RectStorage<T>::usedArea() const
{
    ensureLoaded();
    return m_tree.boundingBox().toRect();
}

template<typename T>
void RectStorage<T>::load(const QList<QPair<Region, T>> &data)
{
    Q_ASSERT(!m_loader);
    m_loader = new RectStorageLoader<T>(this, data);
}

template<typename T>
QVector<QPair<QRectF, T>> RectStorage<T>::currentData(const Region &region) const
{
    ensureLoaded();
    QVector<QPair<QRectF, T>> result;
    Region::ConstIterator end = region.constEnd();
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
        const QRect rect = (*it)->rect();
        QList<QPair<QRectF, T>> pairs = m_tree.intersectingPairs(rect).values();
        for (int i = 0; i < pairs.count(); ++i) {
            // trim the rects
            pairs[i].first = pairs[i].first.intersected(rect);
        }
        // Always add a default value even if there are no pairs.
        result.push_back(qMakePair(QRectF(rect), T()));
        result.append(QVector<QPair<QRectF, T>>::fromList(pairs));
    }
    return result;
}

template<typename T>
void RectStorage<T>::insert(const Region &region, const T &_data)
{
    ensureLoaded();

    if (m_storingUndo)
        m_undoData << currentData(region);

    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        // insert data
        m_tree.insert((*it)->rect(), _data);
        regionChanged((*it)->rect());
    }
}

template<typename T>
void RectStorage<T>::remove(const Region &region, const T &data)
{
    ensureLoaded();

    if (m_storingUndo)
        m_undoData << currentData(region);

    const Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        // remove data
        m_tree.remove((*it)->rect(), data);
        regionChanged((*it)->rect());
    }
}

template<typename T>
void RectStorage<T>::insertRows(int position, int number)
{
    ensureLoaded();
    const QRect invalidRect(1, position, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // process the tree
    QVector<QPair<QRectF, T>> undoData;
    undoData << m_tree.insertRows(position, number);
    if (m_storingUndo)
        m_undoData << undoData;
}

template<typename T>
void RectStorage<T>::insertColumns(int position, int number)
{
    ensureLoaded();
    const QRect invalidRect(position, 1, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // process the tree
    QVector<QPair<QRectF, T>> undoData;
    undoData << m_tree.insertColumns(position, number);
    if (m_storingUndo)
        m_undoData << undoData;
}

template<typename T>
void RectStorage<T>::removeRows(int position, int number)
{
    ensureLoaded();
    const QRect invalidRect(1, position, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // process the tree
    QVector<QPair<QRectF, T>> undoData;
    undoData << m_tree.removeRows(position, number);
    if (m_storingUndo)
        m_undoData << undoData;
}

template<typename T>
void RectStorage<T>::removeColumns(int position, int number)
{
    ensureLoaded();
    const QRect invalidRect(position, 1, KS_colMax, KS_rowMax);
    // invalidate the affected, cached styles
    invalidateCache(invalidRect);
    // process the tree
    QVector<QPair<QRectF, T>> undoData;
    undoData << m_tree.removeColumns(position, number);
    if (m_storingUndo)
        m_undoData << undoData;
}

template<typename T>
void RectStorage<T>::insertShiftRight(const QRect &rect)
{
    ensureLoaded();
    const QRect invalidRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom()));
    QVector<QPair<QRectF, T>> undoData;
    undoData << m_tree.insertShiftRight(rect);
    regionChanged(invalidRect);
    if (m_storingUndo)
        m_undoData << undoData;
}

template<typename T>
void RectStorage<T>::insertShiftDown(const QRect &rect)
{
    ensureLoaded();
    const QRect invalidRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax));
    QVector<QPair<QRectF, T>> undoData;
    undoData << m_tree.insertShiftDown(rect);
    if (m_storingUndo)
        m_undoData << undoData;
    regionChanged(invalidRect);
}

template<typename T>
void RectStorage<T>::removeShiftLeft(const QRect &rect)
{
    ensureLoaded();
    const QRect invalidRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom()));
    QVector<QPair<QRectF, T>> undoData;
    undoData << m_tree.removeShiftLeft(rect);
    if (m_storingUndo)
        m_undoData << undoData;
    regionChanged(invalidRect);
}

template<typename T>
void RectStorage<T>::removeShiftUp(const QRect &rect)
{
    ensureLoaded();
    const QRect invalidRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax));
    QVector<QPair<QRectF, T>> undoData;
    undoData << m_tree.removeShiftUp(rect);
    if (m_storingUndo)
        m_undoData << undoData;
    regionChanged(invalidRect);
}

template<typename T>
void RectStorage<T>::triggerGarbageCollection()
{
    garbageCollection();
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
    if (pairs.isEmpty()) // actually never true, just for sanity
        return;
    int zIndex = pairs.constBegin().key();
    DataPair pair = pairs[zIndex];

    // check whether the default style is placed first
    if (zIndex == currentZIndex && currentPair.second == T() && pair.second == T() && pair.first == currentPair.first) {
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
            if (zIndex == currentZIndex && pair.first == currentPair.first && pair.second == currentPair.second) {
                found = true;
            }
            continue;
        }

        // remove the current pair, if another substyle of the same type,
        // the default style or a named style follows and the rectangle
        // is completely covered
        if (zIndex != currentZIndex && (pair.second == currentPair.second || pair.second == T()) && pair.first.toRect().contains(currentPair.first.toRect())) {
            debugSheets << "RectStorage: removing data at" << Region(currentPair.first.toRect()).name();
            m_tree.remove(currentPair.first.toRect(), currentPair.second);
            break;
        }
    }
    triggerGarbageCollection();
}

template<typename T>
void RectStorage<T>::regionChanged(const QRect &rect)
{
    if (m_loader && !m_loader->isFinished())
        return;
    if (m_map->isLoading())
        return;
    // mark the possible garbage
    // NOTE Stefan: The map may contain multiple indices. The already existing possible garbage has
    // has to be inserted most recently, because it should be accessed first.
    m_possibleGarbage = QMultiMap(m_tree.intersectingPairs(rect)).unite(m_possibleGarbage);
    triggerGarbageCollection();
    // invalidate cache
    invalidateCache(rect);
}

template<typename T>
void RectStorage<T>::invalidateCache(const QRect &invRect)
{
    if (m_loader && !m_loader->isFinished())
        return;
#ifdef CALLIGRA_SHEETS_MT
    QMutexLocker ml(&m_mutex);
#endif
    Region invRegion = Region(invRect);
    const QVector<QRect> rects = m_cachedArea.intersected(invRegion).rects();
    m_cachedArea.removeIntersects(invRegion);
    qHash(QPoint(0, 0), (size_t)0);
    for (const QRect &rect : rects) {
        for (int col = rect.left(); col <= rect.right(); ++col) {
            for (int row = rect.top(); row <= rect.bottom(); ++row)
                m_cache.remove(QPoint(col, row)); // also deletes it
        }
    }
}

template<typename T>
void RectStorage<T>::ensureLoaded() const
{
    if (m_loader) {
        m_loader->waitForFinished();
        delete m_loader;
        const_cast<RectStorage<T> *>(this)->m_loader = nullptr;
    }
}

template<typename T>
const QVector<QPair<QRectF, T>> &RectStorage<T>::undoData() const
{
    return m_undoData;
}

template<typename T>
void RectStorage<T>::resetUndo()
{
    m_undoData.clear();
    storeUndo(false);
}

template<typename T>
RectStorageLoader<T>::RectStorageLoader(RectStorage<T> *storage, const QList<QPair<Region, T>> &data)
    : m_storage(storage)
    , m_data(data)
{
}

template<typename T>
void RectStorageLoader<T>::run()
{
    static int total = 0;
    debugSheets << "Loading conditional styles";
    QElapsedTimer t;
    t.start();

    QList<QPair<Region, T>> treeData;
    typedef QPair<Region, T> TRegion;
    for (const TRegion &tr : m_data) {
        treeData.append(tr);
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
QList<QPair<Region, T>> RectStorageLoader<T>::data() const
{
    return m_data;
}

class CALLIGRA_SHEETS_ENGINE_EXPORT FusionStorage : public QObject, public RectStorage<bool>
{
    Q_OBJECT
public:
    explicit FusionStorage(MapBase *map)
        : QObject()
        , RectStorage<bool>(map)
    {
    }
    FusionStorage(const FusionStorage &other)
        : QObject(other.parent())
        , RectStorage<bool>(other)
    {
    }

protected Q_SLOTS:
    void triggerGarbageCollection() override
    {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, &FusionStorage::garbageCollection);
    }
    void garbageCollection() override
    {
        RectStorage<bool>::garbageCollection();
    }
};

class CALLIGRA_SHEETS_ENGINE_EXPORT MatrixStorage : public QObject, public RectStorage<bool>
{
    Q_OBJECT
public:
    explicit MatrixStorage(MapBase *map)
        : QObject()
        , RectStorage<bool>(map)
    {
    }
    MatrixStorage(const MatrixStorage &other)
        : QObject(other.parent())
        , RectStorage<bool>(other)
    {
    }

protected Q_SLOTS:
    void triggerGarbageCollection() override
    {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, &MatrixStorage::garbageCollection);
    }
    void garbageCollection() override
    {
        RectStorage<bool>::garbageCollection();
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_RECT_STORAGE
