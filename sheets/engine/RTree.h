/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __SHEETS_RTREE__
#define __SHEETS_RTREE__

#include <KoRTree.h>

#include "Region.h"
#include "calligra_sheets_limits.h"

// Use dynamic_cast instead of cached root node
// this is much slower but it is here so it is easy to check that still all works.
// #define DYNAMIC_CAST

namespace Calligra
{
namespace Sheets
{

// Note that the 'this->' prefixes are needed to access members from a template base class.

/**
 * \class RTree
 * \brief An R-Tree template
 * \ingroup Storage
 *
 * An R-Tree template extended by special needs of Calligra Sheets:
 * \li adjusts the rectangles on insertion to avoid unwanted overlapping
 * (caused by different intersection/containment behaviour of QRectF and QRect)
 * \li checks for sane rectangle dimensions
 * \li provides insertion and deletion of columns and rows
 *
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 */
template<typename T>
class RTree : public KoRTree<T>
{
public:
    /**
     * Constructs an empty R-Tree.
     */
    RTree();

    /**
     * Destroys the whole R-Tree.
     */
    ~RTree() override;

    /**
     * @brief Insert data item into the tree
     *
     * This will insert a data item into the tree. If necessary the tree will
     * adjust itself.
     *
     * \note Reimplemented for Calligra Sheets, because of the QRectF behaviour differs from
     * the one of QRect. Intersection or containment for boundary lines or points is
     * not the same, e.g. QRectF(1, 1, 1, 1) intersects QRectF(2, 1, 1, 1) while for
     * QRect it does not. Therefore, this method subtracts 0.1 from the width and
     * height of \p rect .
     *
     * @param data
     * @param rect
     */
    void insert(const QRectF &rect, const T &data) override;

    void load(const QList<QPair<Region, T>> &data);

    void remove(const QRectF &rect, const T &data, int id = -1);

    /**
     * Finds all data items at the location \p point .
     *
     * \param point where the objects have to be in
     *
     * \return objects at the location
     */
    virtual QList<T> contains(const QPointF &point) const;

    /**
     * Finds all data items that cover \p rect completely.
     *
     * \param rect where the objects have to be in
     *
     * \return objects containing the rect
     */
    virtual QList<T> contains(const QRectF &rect) const;

    /**
     * @brief Find all data items which intersects rect
     *
     * \note Reimplemented for Calligra Sheets, because of the QRectF behaviour differs from
     * the one of QRect. Intersection or containment for boundary lines or points is
     * not the same, e.g. QRectF(1, 1, 1, 1) intersects QRectF(2, 1, 1, 1) while for
     * QRect it does not. Therefore, this method subtracts 0.1 from the width and
     * height of \p rect .
     *
     * @param rect where the objects have to be in
     *
     * @return objects intersecting the rect
     */
    QList<T> intersects(const QRectF &rect) const override;

    virtual QMap<int, QPair<QRectF, T>> intersectingPairs(const QRectF &rect) const;

    /**
     * Inserts \p number rows at the position \p position .
     * It extends or shifts rectangles, respectively.
     * Implemented using insertShiftDown.
     * \return the removed rectangle/data pairs
     */
    virtual QVector<QPair<QRectF, T>> insertRows(int position, int number);

    /**
     * Inserts \p number columns at the position \p position .
     * It extends or shifts rectangles, respectively.
     * Implemented using insertShiftRight.
     * \return the removed rectangle/data pairs
     */
    virtual QVector<QPair<QRectF, T>> insertColumns(int position, int number);

    /**
     * Deletes \p number rows at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     * Implemented using removeShiftUp.
     * \return the removed rectangle/data pairs
     */
    virtual QVector<QPair<QRectF, T>> removeRows(int position, int number);

    /**
     * Deletes \p number columns at the position \p position .
     * It shrinks or shifts rectangles, respectively.
     * Implemented using removeShiftLeft.
     * \return the removed rectangle/data pairs
     */
    virtual QVector<QPair<QRectF, T>> removeColumns(int position, int number);

    /**
     * Shifts the rows right of \p rect to the right by the width of \p rect .
     * It extends or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    virtual QVector<QPair<QRectF, T>> insertShiftRight(const QRect &rect);

    /**
     * Shifts the columns at the bottom of \p rect to the bottom by the height of \p rect .
     * It extends or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    virtual QVector<QPair<QRectF, T>> insertShiftDown(const QRect &rect);

    /**
     * Shifts the rows left of \p rect to the left by the width of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    virtual QVector<QPair<QRectF, T>> removeShiftLeft(const QRect &rect);

    /**
     * Shifts the columns on top of \p rect to the top by the height of \p rect .
     * It shrinks or shifts rectangles, respectively.
     * \return the former rectangle/data pairs
     */
    virtual QVector<QPair<QRectF, T>> removeShiftUp(const QRect &rect);

    /**
     * Splits the stored data so that any rectangles crossing over the col-1..col boundary are split in two. Used by shifting operations.
     */
    void splitBeforeColumn(int col);

    /**
     * Splits the stored data so that any rectangles crossing over the row-1..row boundary are split in two. Used by shifting operations.
     */
    void splitBeforeRow(int row);

    /**
     * Checks if the tree is valid. Used in testing.
     */
    bool validate();

    /**
     * Assignment.
     */
    void operator=(const RTree &other);

    /**
     * Returns the bounding box for the entire tree.
     */
    QRectF boundingBox() const
    {
        return this->m_root->boundingBox();
    }

    void clear() override
    {
        KoRTree<T>::clear();
        m_castRoot = dynamic_cast<Node *>(this->m_root);
    }

    QStringList dataDescription() const;

    // disable copy constructor
    RTree(const RTree &other) = delete;

protected:
    class Node;
    class NonLeafNode;
    class LeafNode;

    // factory methods
    LeafNode *createLeafNode(int capacity, int level, typename KoRTree<T>::Node *parent) override
    {
        return new LeafNode(capacity, level, dynamic_cast<Node *>(parent));
    }
    NonLeafNode *createNonLeafNode(int capacity, int level, typename KoRTree<T>::Node *parent) override
    {
        return new NonLeafNode(capacity, level, dynamic_cast<Node *>(parent));
    }

    void adjustTree(typename KoRTree<T>::Node *node1, typename KoRTree<T>::Node *node2) override
    {
        KoRTree<T>::adjustTree(node1, node2);
        m_castRoot = dynamic_cast<Node *>(this->m_root);
    }

    void condenseTree(typename KoRTree<T>::Node *node, QVector<typename KoRTree<T>::Node *> &reinsert) override
    {
        KoRTree<T>::condenseTree(node, reinsert);
        m_castRoot = dynamic_cast<Node *>(this->m_root);
    }

private:
    struct LoadData {
        QRect rect;
        const T *data;
        qreal value;
        LoadData(const QRect &r, const T *d, qreal v)
            : rect(r)
            , data(d)
            , value(v)
        {
        }
    };
    struct LoadDataIndexCompare {
        const QList<LoadData> &m_data;
        LoadDataIndexCompare(const QList<LoadData> &data)
            : m_data(data)
        {
        }
        bool operator()(int a, int b)
        {
            return m_data[a].value < m_data[b].value;
        }
    };
    struct NodeLoadDataIndexCompare {
        const QList<QPair<Node *, qreal>> &m_data;
        NodeLoadDataIndexCompare(const QList<QPair<Node *, qreal>> &data)
            : m_data(data)
        {
        }
        bool operator()(int a, int b)
        {
            return m_data[a].second < m_data[b].second;
        }
    };

    Node *m_castRoot;
};

/**
 * Abstract base class for nodes and leaves.
 */
template<typename T>
class RTree<T>::Node : virtual public KoRTree<T>::Node
{
public:
    Node(int capacity, int level, Node *parent)
        : KoRTree<T>::Node(capacity, level, parent)
    {
    }
    ~Node() override = default;

    void remove(int index) override
    {
        KoRTree<T>::Node::remove(index);
    }
    virtual void remove(const QRectF &rect, const T &data, int id = -1) = 0;
    void contains(const QPointF &point, QMap<int, T> &result) const override = 0;
    virtual void contains(const QRectF &rect, QMap<int, T> &result) const = 0;
    virtual void intersectingPairs(const QRectF &rect, QMap<int, QPair<QRectF, T>> &result) const = 0;
    virtual void insertShiftRight(const QRect &rect, QVector<QPair<QRectF, T>> &res) = 0;
    virtual void insertShiftDown(const QRect &rect, QVector<QPair<QRectF, T>> &res) = 0;
    virtual void removeShiftLeft(const QRect &rect, QVector<QPair<QRectF, T>> &res) = 0;
    virtual void removeShiftUp(const QRect &rect, QVector<QPair<QRectF, T>> &res) = 0;
    // helper for splitBeforeColumn, returns the cut-off data
    virtual void cutBeforeColumn(int col, QVector<QPair<QRectF, T>> &res) = 0;
    // helper for splitBeforeRow, returns the cut-off data
    virtual void cutBeforeRow(int row, QVector<QPair<QRectF, T>> &res) = 0;
    virtual bool validate() = 0;
    const QRectF &childBoundingBox(int index) const override
    {
        return KoRTree<T>::Node::childBoundingBox(index);
    }
    QVector<QRectF> childBoundingBox() const
    {
        return this->m_childBoundingBox;
    }
    virtual QStringList dataDescription() const = 0;

    // disable copy constructor
    Node(const Node &other) = delete;

private:
    friend class NonLeafNode;
};

/**
 * An R-Tree leaf.
 */
template<typename T>
class RTree<T>::LeafNode : public RTree<T>::Node, public KoRTree<T>::LeafNode
{
public:
    LeafNode(int capacity, int level, RTree<T>::Node *parent)
        : KoRTree<T>::Node(capacity, level, parent)
        , RTree<T>::Node(capacity, level, parent)
        , KoRTree<T>::LeafNode(capacity, level, parent)
    {
    }
    ~LeafNode() override = default;

    void remove(int index) override
    {
        KoRTree<T>::LeafNode::remove(index);
    }
    void remove(const T &data) override
    {
        KoRTree<T>::LeafNode::remove(data);
    }
    void remove(const QRectF &rect, const T &data, int id = -1) override;
    void contains(const QPointF &point, QMap<int, T> &result) const override
    {
        KoRTree<T>::LeafNode::contains(point, result);
    }
    void contains(const QRectF &rect, QMap<int, T> &result) const override;
    void intersectingPairs(const QRectF &rect, QMap<int, QPair<QRectF, T>> &result) const override;
    virtual void insertShiftRight(const QRect &rect, QVector<QPair<QRectF, T>> &res) override;
    virtual void insertShiftDown(const QRect &rect, QVector<QPair<QRectF, T>> &res) override;
    virtual void removeShiftLeft(const QRect &rect, QVector<QPair<QRectF, T>> &res) override;
    virtual void removeShiftUp(const QRect &rect, QVector<QPair<QRectF, T>> &res) override;
    // helper for splitBeforeColumn
    virtual void cutBeforeColumn(int col, QVector<QPair<QRectF, T>> &res) override;
    // helper for splitBeforeRow
    virtual void cutBeforeRow(int row, QVector<QPair<QRectF, T>> &res) override;
    virtual bool validate() override;
    virtual void operator=(const LeafNode &other);
    virtual QStringList dataDescription() const override;

    // disable copy constructor
    LeafNode(const LeafNode &other) = delete;

private:
};

/**
 * An R-Tree node.
 */
template<typename T>
class RTree<T>::NonLeafNode : public RTree<T>::Node, public KoRTree<T>::NonLeafNode
{
public:
    NonLeafNode(int capacity, int level, RTree<T>::Node *parent)
        : KoRTree<T>::Node(capacity, level, parent)
        , RTree<T>::Node(capacity, level, parent)
        , KoRTree<T>::NonLeafNode(capacity, level, parent)
    {
    }
    ~NonLeafNode() override = default;

    void remove(int index) override
    {
        KoRTree<T>::NonLeafNode::remove(index);
    }
    void remove(const QRectF &rect, const T &data, int id = -1) override;
    void contains(const QPointF &point, QMap<int, T> &result) const override
    {
        KoRTree<T>::NonLeafNode::contains(point, result);
    }
    void contains(const QRectF &rect, QMap<int, T> &result) const override;
    void intersectingPairs(const QRectF &rect, QMap<int, QPair<QRectF, T>> &result) const override;
    virtual void insertShiftRight(const QRect &rect, QVector<QPair<QRectF, T>> &res) override;
    virtual void insertShiftDown(const QRect &rect, QVector<QPair<QRectF, T>> &res) override;
    virtual void removeShiftLeft(const QRect &rect, QVector<QPair<QRectF, T>> &res) override;
    virtual void removeShiftUp(const QRect &rect, QVector<QPair<QRectF, T>> &res) override;
    // helper for splitBeforeColumn
    virtual void cutBeforeColumn(int row, QVector<QPair<QRectF, T>> &res) override;
    // helper for splitBeforeRow
    virtual void cutBeforeRow(int row, QVector<QPair<QRectF, T>> &res) override;
    virtual void operator=(const NonLeafNode &other);
    virtual bool validate() override;
    virtual QStringList dataDescription() const override;

    // disable copy constructor
    NonLeafNode(const NonLeafNode &other) = delete;
};

/////////////////////////////////////////////////////////////////////////////
// RTree definition
//
template<typename T>
RTree<T>::RTree()
    //        : KoRTree<T>(8, 4)
    : KoRTree<T>(128, 64)
{
    delete this->m_root;
    this->m_root = new LeafNode(this->m_capacity + 1, 0, nullptr);
    m_castRoot = dynamic_cast<Node *>(this->m_root);
}

template<typename T>
RTree<T>::~RTree() = default;

template<typename T>
void RTree<T>::insert(const QRectF &rect, const T &data)
{
    Q_ASSERT(rect.x() - (int)rect.x() == 0.0);
    Q_ASSERT(rect.y() - (int)rect.y() == 0.0);
    Q_ASSERT(rect.height() - (int)rect.height() == 0.0);
    Q_ASSERT(rect.width() - (int)rect.width() == 0.0);
    KoRTree<T>::insert(rect.normalized().adjusted(0, 0, -0.1, -0.1), data);
}

static inline qreal calcLoadingRectValue(const QRectF &r)
{
    QPointF center = r.center();
    // TODO: better value would be hilbert value of center of rect
    return center.x();
}

template<typename T>
void RTree<T>::load(const QList<QPair<Region, T>> &data)
{
    // clear current tree
    clear();

    // make rect->data mapping
    typedef QPair<Region, T> DataRegion;

    QList<LoadData> rectData;
    QVector<int> indices;
    for (const DataRegion &dataRegion : data) {
        for (const QRect &rect : dataRegion.first.rects()) {
            qreal h = calcLoadingRectValue(rect);
            rectData.append(LoadData(rect, &dataRegion.second, h));
            indices.append(indices.size());
        }
    }

    std::sort(indices.begin(), indices.end(), LoadDataIndexCompare(rectData));

    QList<QPair<Node *, qreal>> nodes;
    // create LeafNodes
    for (int i = 0; i < indices.size(); i += this->m_capacity) {
        LeafNode *n = createLeafNode(this->m_capacity + 1, 0, nullptr);
        for (int j = 0; j < this->m_capacity && i + j < indices.size(); j++) {
            const LoadData &d = rectData[indices[i + j]];
            n->insert(QRectF(d.rect).normalized().adjusted(0, 0, -0.1, -0.1), *d.data, LeafNode::dataIdCounter + indices[i + j]);
        }
        n->updateBoundingBox();
        nodes.append(qMakePair<Node *, qreal>(n, calcLoadingRectValue(n->boundingBox())));
    }
    LeafNode::dataIdCounter += indices.size();

    while (nodes.size() > 1) {
        indices.resize(nodes.size());
        for (int i = 0; i < indices.size(); i++)
            indices[i] = i;

        std::sort(indices.begin(), indices.end(), NodeLoadDataIndexCompare(nodes));

        QList<QPair<Node *, qreal>> newNodes;

        for (int i = 0; i < indices.size(); i += this->m_capacity) {
            NonLeafNode *n = createNonLeafNode(this->m_capacity + 1, 0, nullptr);
            for (int j = 0; j < this->m_capacity && i + j < indices.size(); j++) {
                Node *oldNode = nodes[indices[i + j]].first;
                n->insert(oldNode->boundingBox(), oldNode);
            }
            n->updateBoundingBox();
            newNodes.append(qMakePair<Node *, qreal>(n, calcLoadingRectValue(n->boundingBox())));
        }
        nodes = newNodes;
    }

    if (!nodes.isEmpty()) {
        // set root node
        delete this->m_root;
        this->m_root = nodes.first().first;
        m_castRoot = dynamic_cast<Node *>(this->m_root);
    }
}

template<typename T>
void RTree<T>::remove(const QRectF &rect, const T &data, int id)
{
    Q_ASSERT(rect.x() - (int)rect.x() == 0.0);
    Q_ASSERT(rect.y() - (int)rect.y() == 0.0);
    Q_ASSERT(rect.height() - (int)rect.height() == 0.0);
    Q_ASSERT(rect.width() - (int)rect.width() == 0.0);
#ifdef DYNAMIC_CAST
    dynamic_cast<Node *>(this->m_root)->remove(rect.normalized().adjusted(0, 0, -0.1, -0.1), data, id);
#else
    m_castRoot->remove(rect.normalized().adjusted(0, 0, -0.1, -0.1), data, id);
#endif
}

template<typename T>
QList<T> RTree<T>::contains(const QPointF &point) const
{
    return KoRTree<T>::contains(point);
}

template<typename T>
QList<T> RTree<T>::contains(const QRectF &rect) const
{
    Q_ASSERT(rect.x() - (int)rect.x() == 0.0);
    Q_ASSERT(rect.y() - (int)rect.y() == 0.0);
    Q_ASSERT(rect.height() - (int)rect.height() == 0.0);
    Q_ASSERT(rect.width() - (int)rect.width() == 0.0);
    QMap<int, T> result;
#ifdef DYNAMIC_CAST
    dynamic_cast<Node *>(this->m_root)->contains(rect.normalized().adjusted(0, 0, -0.1, -0.1), result);
#else
    m_castRoot->contains(rect.normalized().adjusted(0, 0, -0.1, -0.1), result);
#endif
    return result.values();
}

template<typename T>
QList<T> RTree<T>::intersects(const QRectF &rect) const
{
    Q_ASSERT(rect.x() - (int)rect.x() == 0.0);
    Q_ASSERT(rect.y() - (int)rect.y() == 0.0);
    Q_ASSERT(rect.height() - (int)rect.height() == 0.0);
    Q_ASSERT(rect.width() - (int)rect.width() == 0.0);
    return KoRTree<T>::intersects(rect.normalized().adjusted(0, 0, -0.1, -0.1));
}

template<typename T>
QMap<int, QPair<QRectF, T>> RTree<T>::intersectingPairs(const QRectF &rect) const
{
    Q_ASSERT(rect.x() - (int)rect.x() == 0.0);
    Q_ASSERT(rect.y() - (int)rect.y() == 0.0);
    Q_ASSERT(rect.height() - (int)rect.height() == 0.0);
    Q_ASSERT(rect.width() - (int)rect.width() == 0.0);
    QMap<int, QPair<QRectF, T>> result;
#ifdef DYNAMIC_CAST
    dynamic_cast<Node *>(this->m_root)->intersectingPairs(rect.normalized().adjusted(0, 0, -0.1, -0.1), result);
#else
    m_castRoot->intersectingPairs(rect.normalized().adjusted(0, 0, -0.1, -0.1), result);
#endif
    return result;
}

template<typename T>
QVector<QPair<QRectF, T>> RTree<T>::insertRows(int position, int number)
{
    Q_ASSERT(position >= 1);
    Q_ASSERT(position <= KS_rowMax);
    QVector<QPair<QRectF, T>> res;
    if (position < 1 || position > KS_rowMax)
        return res;
    return insertShiftDown(QRect(1, position, KS_colMax, number));
}

template<typename T>
QVector<QPair<QRectF, T>> RTree<T>::insertColumns(int position, int number)
{
    Q_ASSERT(position >= 1);
    Q_ASSERT(position <= KS_colMax);
    QVector<QPair<QRectF, T>> res;
    if (position < 1 || position > KS_colMax)
        return res;
    return insertShiftRight(QRect(position, 1, number, KS_rowMax));
}

template<typename T>
QVector<QPair<QRectF, T>> RTree<T>::removeRows(int position, int number)
{
    Q_ASSERT(position >= 1);
    Q_ASSERT(position <= KS_rowMax);
    QVector<QPair<QRectF, T>> res;
    if (position < 1 || position > KS_rowMax)
        return res;
    return removeShiftUp(QRect(1, position, KS_colMax, number));
}

template<typename T>
QVector<QPair<QRectF, T>> RTree<T>::removeColumns(int position, int number)
{
    Q_ASSERT(position >= 1);
    Q_ASSERT(position <= KS_colMax);
    QVector<QPair<QRectF, T>> res;
    if (position < 1 || position > KS_colMax)
        return res;
    return removeShiftLeft(QRect(position, 1, number, KS_rowMax));
}

template<typename T>
QVector<QPair<QRectF, T>> RTree<T>::insertShiftRight(const QRect &r)
{
    const QRect rect(r.normalized());
    QVector<QPair<QRectF, T>> res;
    if (rect.right() < 1 || rect.left() > KS_colMax)
        return res;

    // Split the data along the left column and both rows
    splitBeforeColumn(r.left());
    splitBeforeRow(r.top());
    splitBeforeRow(r.bottom() + 1);

    // Go over all the data, increasing anything that's >= the left column by the width
    // Return excess data that was cut off
#ifdef DYNAMIC_CAST
    dynamic_cast<Node *>(this->m_root)->insertShiftRight(r, res);
#else
    m_castRoot->insertShiftRight(r, res);
#endif
    return res;
}

template<typename T>
QVector<QPair<QRectF, T>> RTree<T>::insertShiftDown(const QRect &r)
{
    const QRect rect(r.normalized());
    QVector<QPair<QRectF, T>> res;
    if (rect.top() < 1 || rect.top() > KS_rowMax)
        return QVector<QPair<QRectF, T>>();

    // Split the data along the top row and both columns
    splitBeforeRow(r.top());
    splitBeforeColumn(r.left());
    splitBeforeColumn(r.right() + 1);

    // Go over all the data, increasing anything that's >= the top row by the height
    // Return excess data that was cut off
#ifdef DYNAMIC_CAST
    dynamic_cast<Node *>(this->m_root)->insertShiftDown(r, res);
#else
    m_castRoot->insertShiftDown(r, res);
#endif
    return res;
}

template<typename T>
QVector<QPair<QRectF, T>> RTree<T>::removeShiftLeft(const QRect &r)
{
    const QRect rect(r.normalized());
    QVector<QPair<QRectF, T>> res;
    if (rect.left() < 1 || rect.left() > KS_colMax)
        return QVector<QPair<QRectF, T>>();

    // Split the data along the left column and both rows
    splitBeforeColumn(r.left());
    splitBeforeRow(r.top());
    splitBeforeRow(r.bottom() + 1);

    // Go over all the data, decreasing anything that's >= the left column by the width
    // Return excess data that was cut off
#ifdef DYNAMIC_CAST
    dynamic_cast<Node *>(this->m_root)->removeShiftLeft(r, res);
#else
    m_castRoot->removeShiftLeft(r, res);
#endif
    return res;
}

template<typename T>
QVector<QPair<QRectF, T>> RTree<T>::removeShiftUp(const QRect &r)
{
    const QRect rect(r.normalized());
    QVector<QPair<QRectF, T>> res;
    if (rect.top() < 1 || rect.top() > KS_rowMax)
        return QVector<QPair<QRectF, T>>();

    // Split the data along the top row and both columns
    splitBeforeRow(r.top());
    splitBeforeColumn(r.left());
    splitBeforeColumn(r.right() + 1);

    // Go over all the data, decreasing anything that's >= the top row by the height
    // Return excess data that was cut off
#ifdef DYNAMIC_CAST
    dynamic_cast<Node *>(this->m_root)->removeShiftUp(r, res);
#else
    m_castRoot->removeShiftUp(r, res);
#endif
    return res;
}

template<typename T>
void RTree<T>::splitBeforeColumn(int col)
{
    QVector<QPair<QRectF, T>> cutData;

#ifdef DYNAMIC_CAST
    dynamic_cast<Node *>(this->m_root)->cutBeforeColumn(col, cutData);
#else
    m_castRoot->cutBeforeColumn(col, cutData);
#endif

    // and insert the cut data
    for (int i = 0; i < cutData.count(); ++i) {
        insert(cutData[i].first, cutData[i].second);
    }
}

template<typename T>
void RTree<T>::splitBeforeRow(int row)
{
    QVector<QPair<QRectF, T>> cutData;

#ifdef DYNAMIC_CAST
    dynamic_cast<Node *>(this->m_root)->cutBeforeRow(row, cutData);
#else
    m_castRoot->cutBeforeRow(row, cutData);
#endif

    // and insert the cut data
    for (int i = 0; i < cutData.count(); ++i) {
        insert(cutData[i].first, cutData[i].second);
    }
}

template<typename T>
bool RTree<T>::validate()
{
#ifdef DYNAMIC_CAST
    return dynamic_cast<Node *>(this->m_root)->validate();
#else
    return m_castRoot->validate();
#endif
}

template<typename T>
QStringList RTree<T>::dataDescription() const
{
#ifdef DYNAMIC_CAST
    return dynamic_cast<Node *>(this->m_root)->dataDescription();
#else
    return m_castRoot->dataDescription();
#endif
}

template<typename T>
void RTree<T>::operator=(const RTree<T> &other)
{
    this->m_capacity = other.m_capacity;
    this->m_minimum = other.m_minimum;
    delete this->m_root;
    if (other.m_root->isLeaf()) {
        this->m_root = new LeafNode(this->m_capacity + 1, 0, nullptr);
        *dynamic_cast<LeafNode *>(this->m_root) = *dynamic_cast<LeafNode *>(other.m_root);
    } else {
        this->m_root = new NonLeafNode(this->m_capacity + 1, 0, nullptr);
        *dynamic_cast<NonLeafNode *>(this->m_root) = *dynamic_cast<NonLeafNode *>(other.m_root);
    }
    m_castRoot = dynamic_cast<Node *>(this->m_root);
}

/////////////////////////////////////////////////////////////////////////////
// RTree<T>::LeafNode definition
//
template<typename T>
void RTree<T>::LeafNode::remove(const QRectF &rect, const T &data, int id)
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i] == rect && this->m_data[i] == data && (id == -1 || this->m_dataIds[i] == id)) {
            KoRTree<T>::LeafNode::remove(i);
            break;
        }
    }
}

template<typename T>
void RTree<T>::LeafNode::contains(const QRectF &rect, QMap<int, T> &result) const
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i].contains(rect)) {
            result.insert(this->m_dataIds[i], this->m_data[i]);
        }
    }
}

template<typename T>
void RTree<T>::LeafNode::intersectingPairs(const QRectF &rect, QMap<int, QPair<QRectF, T>> &result) const
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i].intersects(rect)) {
            QRectF rect = this->m_childBoundingBox[i].adjusted(0, 0, 0.1, 0.1);
            result.insert(this->m_dataIds[i], qMakePair(rect, this->m_data[i]));
        }
    }
}

template<typename T>
void RTree<T>::LeafNode::insertShiftRight(const QRect &rect, QVector<QPair<QRectF, T>> &res)
{
    double x1 = this->m_boundingBox.left();
    double x2 = this->m_boundingBox.right();
    double y1 = this->m_boundingBox.top();
    double y2 = this->m_boundingBox.bottom();
    int w = ((int)rect.width());

    if (rect.left() > x2)
        return;
    if (rect.top() > y2)
        return;
    if (rect.bottom() < y1)
        return;

    double bound1 = -1, bound2 = -1;

    for (int i = 0; i < this->childCount(); ++i) {
        x1 = this->m_childBoundingBox[i].left();
        x2 = this->m_childBoundingBox[i].right();
        y1 = this->m_childBoundingBox[i].top();
        y2 = this->m_childBoundingBox[i].bottom();
        if ((bound1 == -1) || (bound1 > x1))
            bound1 = x1;
        if ((bound2 == -1) || (bound2 < x2))
            bound2 = x2;

        if (rect.left() > x2)
            continue;
        if (rect.top() > y2)
            continue;
        if (rect.bottom() < y1)
            continue;

        double newx1 = x1 + w;
        double newx2 = x2 + w;

        if (newx1 > KS_colMax) { // We're cutting the whole range.
            res.append(qMakePair(this->m_childBoundingBox[i], this->m_data[i]));
            KoRTree<T>::LeafNode::remove(i--);
            continue;
        }

        if (newx2 > KS_colMax - 0.1) { // We're cutting a portion of this range.
            newx2 = KS_colMax - 0.1;
            // Let's just add the whole range to avoid too much fragmentation.
            res.append(qMakePair(this->m_childBoundingBox[i], this->m_data[i]));
        }

        this->m_childBoundingBox[i].setLeft(newx1);
        this->m_childBoundingBox[i].setRight(newx2);

        if ((bound1 == -1) || (bound1 > newx1))
            bound1 = newx1;
        if ((bound2 == -1) || (bound2 < newx2))
            bound2 = newx2;
    }
    if (bound1 > -1) {
        this->m_boundingBox.setLeft(bound1);
        this->m_boundingBox.setRight(bound2);
    }
}

template<typename T>
void RTree<T>::LeafNode::insertShiftDown(const QRect &rect, QVector<QPair<QRectF, T>> &res)
{
    double x1 = this->m_boundingBox.left();
    double x2 = this->m_boundingBox.right();
    double y1 = this->m_boundingBox.top();
    double y2 = this->m_boundingBox.bottom();
    int h = ((int)rect.height());

    if (rect.top() > y2)
        return;
    if (rect.left() > x2)
        return;
    if (rect.right() < x1)
        return;

    double bound1 = -1, bound2 = -1;

    for (int i = 0; i < this->childCount(); ++i) {
        x1 = this->m_childBoundingBox[i].left();
        x2 = this->m_childBoundingBox[i].right();
        y1 = this->m_childBoundingBox[i].top();
        y2 = this->m_childBoundingBox[i].bottom();
        if ((bound1 == -1) || (bound1 > y1))
            bound1 = y1;
        if ((bound2 == -1) || (bound2 < y2))
            bound2 = y2;

        if (rect.top() > y2)
            continue;
        if (rect.left() > x2)
            continue;
        if (rect.right() < x1)
            continue;

        double newy1 = y1 + h;
        double newy2 = y2 + h;

        if (newy1 > KS_rowMax) { // We're cutting the whole range.
            res.append(qMakePair(this->m_childBoundingBox[i], this->m_data[i]));
            KoRTree<T>::LeafNode::remove(i--);
            continue;
        }

        if (newy2 > KS_rowMax - 0.1) { // We're cutting a portion of this range.
            newy2 = KS_rowMax - 0.1;
            // Let's just add the whole range to avoid too much fragmentation.
            res.append(qMakePair(this->m_childBoundingBox[i], this->m_data[i]));
        }

        this->m_childBoundingBox[i].setTop(newy1);
        this->m_childBoundingBox[i].setBottom(newy2);

        if ((bound1 == -1) || (bound1 > newy1))
            bound1 = newy1;
        if ((bound2 == -1) || (bound2 < newy2))
            bound2 = newy2;
    }

    if (bound1 > -1) {
        this->m_boundingBox.setTop(bound1);
        this->m_boundingBox.setBottom(bound2);
    }
}

template<typename T>
void RTree<T>::LeafNode::removeShiftLeft(const QRect &rect, QVector<QPair<QRectF, T>> &res)
{
    double x1 = this->m_boundingBox.left();
    double x2 = this->m_boundingBox.right();
    double y1 = this->m_boundingBox.top();
    double y2 = this->m_boundingBox.bottom();
    int w = ((int)rect.width());

    if (rect.left() > x2)
        return;
    if (rect.top() > y2)
        return;
    if (rect.bottom() < y1)
        return;

    double bound1 = -1, bound2 = -1;

    for (int i = 0; i < this->childCount(); ++i) {
        x1 = this->m_childBoundingBox[i].left();
        x2 = this->m_childBoundingBox[i].right();
        y1 = this->m_childBoundingBox[i].top();
        y2 = this->m_childBoundingBox[i].bottom();
        if ((bound1 == -1) || (bound1 > x1))
            bound1 = x1;
        if ((bound2 == -1) || (bound2 < x2))
            bound2 = x2;

        if (rect.left() > x2)
            continue;
        if (rect.top() > y2)
            continue;
        if (rect.bottom() < y1)
            continue;

        // This inserts the entire region instead of the cut off portion only, but that's okay and we avoid too much fragmentation.
        const QRectF oldRect(this->m_childBoundingBox[i]);
        QRect mrect = this->m_childBoundingBox[i].toRect();
        int shift = qMin(mrect.left() - rect.left(), w);
        int cut = qMax(0, rect.left() + w - mrect.left());
        this->m_childBoundingBox[i].adjust(-shift, 0, -shift - cut, 0);
        res.append(qMakePair(oldRect, this->m_data[i]));

        if (this->m_childBoundingBox[i].isEmpty()) {
            KoRTree<T>::LeafNode::remove(i--);
            continue;
        }

        double newx1 = this->m_childBoundingBox[i].left();
        double newx2 = this->m_childBoundingBox[i].right();
        if ((bound1 == -1) || (bound1 > newx1))
            bound1 = newx1;
        if ((bound2 == -1) || (bound2 < newx2))
            bound2 = newx2;
    }

    if (bound1 > -1) {
        this->m_boundingBox.setLeft(bound1);
        this->m_boundingBox.setRight(bound2);
    }
}

template<typename T>
void RTree<T>::LeafNode::removeShiftUp(const QRect &rect, QVector<QPair<QRectF, T>> &res)
{
    double x1 = this->m_boundingBox.left();
    double x2 = this->m_boundingBox.right();
    double y2 = this->m_boundingBox.bottom();
    int h = ((int)rect.height());

    if (rect.top() > y2)
        return;
    if (rect.left() > x2)
        return;
    if (rect.right() < x1)
        return;

    double bound1 = -1, bound2 = -1;

    for (int i = 0; i < this->childCount(); ++i) {
        x1 = this->m_childBoundingBox[i].left();
        x2 = this->m_childBoundingBox[i].right();
        double y1 = this->m_childBoundingBox[i].top();
        y2 = this->m_childBoundingBox[i].bottom();
        if ((bound1 == -1) || (bound1 > y1))
            bound1 = y1;
        if ((bound2 == -1) || (bound2 < y2))
            bound2 = y2;

        if (rect.top() > y2)
            continue;
        if (rect.left() > x2)
            continue;
        if (rect.right() < x1)
            continue;

        // This inserts the entire region instead of the cut off portion only, but that's okay and we avoid too much fragmentation.
        const QRectF oldRect(this->m_childBoundingBox[i]);
        QRect mrect = this->m_childBoundingBox[i].toRect();
        int shift = qMin(mrect.top() - rect.top(), h);
        int cut = qMax(0, rect.top() + h - mrect.top());
        this->m_childBoundingBox[i].adjust(0, -shift, 0, -shift - cut);
        res.append(qMakePair(oldRect, this->m_data[i]));

        if (this->m_childBoundingBox[i].isEmpty()) {
            KoRTree<T>::LeafNode::remove(i--);
            continue;
        }

        double newy1 = this->m_childBoundingBox[i].top();
        double newy2 = this->m_childBoundingBox[i].bottom();
        if ((bound1 == -1) || (bound1 > newy1))
            bound1 = newy1;
        if ((bound2 == -1) || (bound2 < newy2))
            bound2 = newy2;
    }

    if (bound1 > -1) {
        this->m_boundingBox.setTop(bound1);
        this->m_boundingBox.setBottom(bound2);
    }
}

template<typename T>
bool RTree<T>::LeafNode::validate()
{
    double x1 = this->m_boundingBox.left();
    double x2 = this->m_boundingBox.right();
    double y1 = this->m_boundingBox.top();
    double y2 = this->m_boundingBox.bottom();

    // All the childs must be within the boundaries
    for (int i = 0; i < this->childCount(); ++i) {
        if (this->m_childBoundingBox[i].top() < y1)
            return false;
        if (this->m_childBoundingBox[i].bottom() > y2)
            return false;
        if (this->m_childBoundingBox[i].left() < x1)
            return false;
        if (this->m_childBoundingBox[i].right() > x2)
            return false;
    }
    return true;
}

template<typename T>
void RTree<T>::LeafNode::operator=(const LeafNode &other)
{
    // leave alone the m_parent
    this->m_boundingBox = other.m_boundingBox;
    this->m_childBoundingBox = other.m_childBoundingBox;
    this->m_counter = other.m_counter;
    this->m_place = other.m_place;
#ifdef CALLIGRA_RTREE_DEBUG
    this->m_nodeId = other.m_nodeId;
#endif
    this->m_level = other.m_level;
    this->m_data = other.m_data;
    this->m_dataIds = other.m_dataIds;
}

/////////////////////////////////////////////////////////////////////////////
// RTree<T>::NonLeafNode definition
//
template<typename T>
void RTree<T>::NonLeafNode::remove(const QRectF &rect, const T &data, int id)
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i].contains(rect)) {
            dynamic_cast<Node *>(this->m_childs[i])->remove(rect, data, id);
        }
    }
}

template<typename T>
void RTree<T>::NonLeafNode::contains(const QRectF &rect, QMap<int, T> &result) const
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i].intersects(rect)) {
            this->m_childs[i]->intersects(rect, result);
        }
    }
}

template<typename T>
void RTree<T>::NonLeafNode::intersectingPairs(const QRectF &rect, QMap<int, QPair<QRectF, T>> &result) const
{
    for (int i = 0; i < this->m_counter; ++i) {
        if (this->m_childBoundingBox[i].intersects(rect)) {
            dynamic_cast<Node *>(this->m_childs[i])->intersectingPairs(rect, result);
        }
    }
}

template<typename T>
void RTree<T>::NonLeafNode::insertShiftRight(const QRect &rect, QVector<QPair<QRectF, T>> &res)
{
    if (rect.left() > this->m_boundingBox.right())
        return;
    if (rect.top() > this->m_boundingBox.bottom())
        return;
    if (rect.bottom() < this->m_boundingBox.top())
        return;

    double bound1 = -1, bound2 = -1;

    for (int i = 0; i < this->childCount(); ++i) {
        Node *node = dynamic_cast<Node *>(this->m_childs[i]);
        node->insertShiftRight(rect, res);
        this->m_childBoundingBox[i] = node->m_boundingBox;
        double x1 = this->m_childBoundingBox[i].left();
        double x2 = this->m_childBoundingBox[i].right();
        if ((bound1 == -1) || (bound1 > x1))
            bound1 = x1;
        if ((bound2 == -1) || (bound2 < x2))
            bound2 = x2;
    }

    if (bound1 > -1) {
        this->m_boundingBox.setLeft(bound1);
        this->m_boundingBox.setRight(bound2);
    }
}

template<typename T>
void RTree<T>::NonLeafNode::insertShiftDown(const QRect &rect, QVector<QPair<QRectF, T>> &res)
{
    if (rect.top() > this->m_boundingBox.bottom())
        return;
    if (rect.left() > this->m_boundingBox.right())
        return;
    if (rect.right() < this->m_boundingBox.left())
        return;

    double bound1 = -1, bound2 = -1;

    for (int i = 0; i < this->childCount(); ++i) {
        Node *node = dynamic_cast<Node *>(this->m_childs[i]);
        node->insertShiftDown(rect, res);
        this->m_childBoundingBox[i] = node->m_boundingBox;
        double y1 = this->m_childBoundingBox[i].top();
        double y2 = this->m_childBoundingBox[i].bottom();
        if ((bound1 == -1) || (bound1 > y1))
            bound1 = y1;
        if ((bound2 == -1) || (bound2 < y2))
            bound2 = y2;
    }

    if (bound1 > -1) {
        this->m_boundingBox.setTop(bound1);
        this->m_boundingBox.setBottom(bound2);
    }
}

template<typename T>
void RTree<T>::NonLeafNode::removeShiftLeft(const QRect &rect, QVector<QPair<QRectF, T>> &res)
{
    if (rect.left() > this->m_boundingBox.right())
        return;
    if (rect.top() > this->m_boundingBox.bottom())
        return;
    if (rect.bottom() < this->m_boundingBox.top())
        return;

    double bound1 = -1, bound2 = -1;

    for (int i = 0; i < this->childCount(); ++i) {
        Node *node = dynamic_cast<Node *>(this->m_childs[i]);
        node->removeShiftLeft(rect, res);
        this->m_childBoundingBox[i] = node->m_boundingBox;
        double x1 = this->m_childBoundingBox[i].left();
        double x2 = this->m_childBoundingBox[i].right();
        if ((bound1 == -1) || (bound1 > x1))
            bound1 = x1;
        if ((bound2 == -1) || (bound2 < x2))
            bound2 = x2;
    }

    if (bound1 > -1) {
        this->m_boundingBox.setLeft(bound1);
        this->m_boundingBox.setRight(bound2);
    }
}

template<typename T>
void RTree<T>::NonLeafNode::removeShiftUp(const QRect &rect, QVector<QPair<QRectF, T>> &res)
{
    if (rect.top() > this->m_boundingBox.bottom())
        return;
    if (rect.left() > this->m_boundingBox.right())
        return;
    if (rect.right() < this->m_boundingBox.left())
        return;

    double bound1 = -1, bound2 = -1;

    for (int i = 0; i < this->childCount(); ++i) {
        Node *node = dynamic_cast<Node *>(this->m_childs[i]);
        node->removeShiftUp(rect, res);
        this->m_childBoundingBox[i] = node->m_boundingBox;
        double y1 = this->m_childBoundingBox[i].top();
        double y2 = this->m_childBoundingBox[i].bottom();
        if ((bound1 == -1) || (bound1 > y1))
            bound1 = y1;
        if ((bound2 == -1) || (bound2 < y2))
            bound2 = y2;
    }

    if (bound1 > -1) {
        this->m_boundingBox.setTop(bound1);
        this->m_boundingBox.setBottom(bound2);
    }
}

template<typename T>
bool RTree<T>::NonLeafNode::validate()
{
    double x1 = this->m_boundingBox.left();
    double x2 = this->m_boundingBox.right();
    double y1 = this->m_boundingBox.top();
    double y2 = this->m_boundingBox.bottom();

    // All the childs must be within the boundaries
    for (int i = 0; i < this->childCount(); ++i) {
        if (this->m_childBoundingBox[i].top() < y1)
            return false;
        if (this->m_childBoundingBox[i].bottom() > y2)
            return false;
        if (this->m_childBoundingBox[i].left() < x1)
            return false;
        if (this->m_childBoundingBox[i].right() > x2)
            return false;
        if (!dynamic_cast<Node *>(this->m_childs[i])->validate())
            return false;
    }
    return true;
}

template<typename T>
void RTree<T>::NonLeafNode::operator=(const NonLeafNode &other)
{
    // leave alone the m_parent
    this->m_boundingBox = other.m_boundingBox;
    this->m_childBoundingBox = other.childBoundingBox();
    this->m_counter = other.m_counter;
    this->m_place = other.m_place;
#ifdef CALLIGRA_RTREE_DEBUG
    this->m_nodeId = other.m_nodeId;
#endif
    this->m_level = other.m_level;
    for (int i = 0; i < other.m_counter; ++i) {
        if (other.m_childs[i]->isLeaf()) {
            LeafNode *child = dynamic_cast<LeafNode *>(other.m_childs[i]);
            this->m_childs[i] = new LeafNode(child->childBoundingBox().size(), child->level(), this);
            *dynamic_cast<LeafNode *>(this->m_childs[i]) = *child;
        } else {
            NonLeafNode *child = dynamic_cast<NonLeafNode *>(other.m_childs[i]);
            this->m_childs[i] = new NonLeafNode(child->childBoundingBox().size(), child->level(), this);
            *dynamic_cast<NonLeafNode *>(this->m_childs[i]) = *child;
        }
    }
}

template<typename T>
void RTree<T>::NonLeafNode::cutBeforeColumn(int col, QVector<QPair<QRectF, T>> &res)
{
    // Not intersecting - nothing to do
    if ((col > this->m_boundingBox.right()) || (col < this->m_boundingBox.left()))
        return;

    for (int i = 0; i < this->childCount(); ++i) {
        QRectF rect = this->m_childBoundingBox[i];
        if ((col > rect.right()) || (col < rect.left()))
            continue;
        this->m_childBoundingBox[i].setRight(col - 0.1);
        dynamic_cast<Node *>(this->m_childs[i])->cutBeforeColumn(col, res);
    }
}

template<typename T>
void RTree<T>::NonLeafNode::cutBeforeRow(int row, QVector<QPair<QRectF, T>> &res)
{
    // Not intersecting - nothing to do
    if ((row > this->m_boundingBox.bottom()) || (row < this->m_boundingBox.top()))
        return;

    for (int i = 0; i < this->childCount(); ++i) {
        QRectF rect = this->m_childBoundingBox[i];
        if ((row > rect.bottom()) || (row < rect.top()))
            continue;
        this->m_childBoundingBox[i].setBottom(row - 0.1);
        dynamic_cast<Node *>(this->m_childs[i])->cutBeforeRow(row, res);
    }
}

template<typename T>
void RTree<T>::LeafNode::cutBeforeColumn(int col, QVector<QPair<QRectF, T>> &res)
{
    for (int i = 0; i < this->m_counter; ++i) {
        QRectF rect = this->m_childBoundingBox[i];
        if ((rect.left() >= col) || (rect.right() <= col))
            continue;

        this->m_childBoundingBox[i].setRight(col - 0.1);
        QRectF cutRect = QRectF(QPointF(col, rect.top()), QPointF(rect.right() + 0.1, rect.bottom() + 0.1));
        res.append(qMakePair(cutRect, this->m_data[i]));
    }
}

template<typename T>
void RTree<T>::LeafNode::cutBeforeRow(int row, QVector<QPair<QRectF, T>> &res)
{
    for (int i = 0; i < this->m_counter; ++i) {
        QRectF rect = this->m_childBoundingBox[i];
        if ((rect.top() >= row) || (rect.bottom() <= row))
            continue;

        this->m_childBoundingBox[i].setBottom(row - 0.1);
        QRectF cutRect = QRectF(QPointF(rect.left(), row), QPointF(rect.right() + 0.1, rect.bottom() + 0.1));
        res.append(qMakePair(cutRect, this->m_data[i]));
    }
}

template<typename T>
QStringList RTree<T>::NonLeafNode::dataDescription() const
{
    QRectF rect = this->m_boundingBox;
    QStringList res;
    res.append("- " + QString::number(this->childCount()) + " children at " + QString::number(rect.top()) + "/" + QString::number(rect.left()) + " - "
               + QString::number(rect.bottom()) + "/" + QString::number(rect.right()));
    for (int i = 0; i < this->childCount(); ++i) {
        QStringList child = dynamic_cast<Node *>(this->m_childs[i])->dataDescription();
        for (QString s : child)
            res.append("  " + s);
    }
    return res;
}

template<typename T>
QStringList RTree<T>::LeafNode::dataDescription() const
{
    QRectF rect = this->m_boundingBox;
    QString res = "- " + QString::number(this->m_counter) + " items at " + QString::number(rect.top()) + "/" + QString::number(rect.left()) + " - "
        + QString::number(rect.bottom()) + "/" + QString::number(rect.right());

    return QStringList(res);
}

} // namespace Sheets
} // namespace Calligra

#endif // __SHEETS_RTREE__
