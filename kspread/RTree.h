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

#ifndef KSPREAD_RTREE
#define KSPREAD_RTREE

#include <QMultiMap>
#include <QRect>
#include <QString>
#include <QVector>

#include <kdebug.h>

#include <KoRTree.h>

#include "Global.h"

namespace KSpread
{

/**
 * \class RTree
 * \brief An R-Tree template
 *
 * An R-Tree template extended by special needs of KSpread:
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
     * Column/row insertion mode.
     */
    enum InsertMode
    {
        /// default insertion mode
        DefaultInsertMode = 0,
        /**
         * Shifts the rectangles, starting from the column/row preceding the
         * insertion position.
         * Thus, the inserted columns/rows carry the same data as the previous
         * column/row.
         */
        CopyPrevious = DefaultInsertMode,
        /**
         * Shifts the rectangles, starting from the column/row at the insertion
         * position.
         * Thus, the inserted columns/rows carry the same data as the current
         * column/row.
         */
        CopyCurrent,
        /**
         * Splits the rectangles at the insertion column/row position.
         * Thus, the inserted columns/rows do not carry any data.
         */
        CopyNone
    };

  /**
   * Constructs an empty R-Tree.
   */
  RTree();

  /**
   * Destroys the whole R-Tree.
   */
  virtual ~RTree();

  /**
   * @brief Insert data item into the tree
   *
   * This will insert a data item into the tree. If necessary the tree will
   * adjust itself.
   *
   * \note Reimplemented for KSpread, because of the QRectF behaviour differs from
   * the one of QRect. Intersection or containment for boundary lines or points is
   * not the same, e.g. QRectF(1, 1, 1, 1) intersects QRectF(2, 1, 1, 1) while for
   * QRect it does not. Therefore, this method substracts 0.1 from the width and
   * height of \p rect .
   *
   * @param data
   * @param rect
   */
  virtual void insert( const QRectF& rect, const T& data );

  void remove( const QRectF& rect, const T& data );

  /**
   * Finds all data items at the location \p point .
   *
   * \param point where the objects have to be in
   *
   * \return objects at the location
   */
  virtual QList<T> contains( const QPointF& point ) const;

  /**
   * Finds all data items that cover \p rect completely.
   *
   * \param rect where the objects have to be in
   *
   * \return objects containing the rect
   */
  virtual QList<T> contains( const QRectF& rect ) const;

  /**
   * @brief Find all data items which intersects rect
   *
   * \note Reimplemented for KSpread, because of the QRectF behaviour differs from
   * the one of QRect. Intersection or containment for boundary lines or points is
   * not the same, e.g. QRectF(1, 1, 1, 1) intersects QRectF(2, 1, 1, 1) while for
   * QRect it does not. Therefore, this method substracts 0.1 from the width and
   * height of \p rect .
   *
   * @param rect where the objects have to be in
   *
   * @return objects intersecting the rect
   */
  virtual QList<T> intersects( const QRectF& rect ) const;

  virtual QList< QPair<QRectF,T> > intersectingPairs( const QRectF& rect ) const;

  /**
   * Inserts \p number rows at the position \p position .
   * It extends or shifts rectangles, respectively.
   */
  virtual void insertRows(int position, int number, InsertMode mode = DefaultInsertMode);

  /**
   * Inserts \p number columns at the position \p position .
   * It extends or shifts rectangles, respectively.
   */
  virtual void insertColumns(int position, int number, InsertMode mode = DefaultInsertMode);

  /**
   * Deletes \p number rows at the position \p position .
   * It shrinks or shifts rectangles, respectively.
   */
  virtual void deleteRows(int position, int number, InsertMode mode = DefaultInsertMode);

  /**
   * Deletes \p number columns at the position \p position .
   * It shrinks or shifts rectangles, respectively.
   */
  virtual void deleteColumns(int position, int number, InsertMode mode = DefaultInsertMode);

protected:
  class Node;
  class NoneLeafNode;
  class LeafNode;

  // factory methods
  virtual LeafNode* createLeafNode( int capacity, int level, typename KoRTree<T>::Node * parent )
  {
    return new LeafNode( capacity, level, dynamic_cast<Node*>(parent) );
  }
  virtual NoneLeafNode* createNoneLeafNode( int capacity, int level, typename KoRTree<T>::Node * parent )
  {
    return new NoneLeafNode( capacity, level, dynamic_cast<Node*>(parent) );
  }
};

/**
 * Abstract base class for nodes and leaves.
 */
template<typename T>
class RTree<T>::Node : virtual public KoRTree<T>::Node
{
public:
  Node(int capacity, int level, Node * parent)
  : KoRTree<T>::Node(capacity, level, parent) {}
  virtual ~Node() {}

  virtual void remove( const QRectF& rect, const T& data ) = 0;
  virtual void contains( const QRectF& rect, QMap<int,T>& result ) const = 0;
  virtual void intersectingPairs( const QRectF& rect, QMap<int,QPair<QRectF,T> >& result ) const = 0;
  virtual void insertRows(int position, int number) = 0;
  virtual void insertColumns(int position, int number) = 0;
  virtual void deleteRows(int position, int number) = 0;
  virtual void deleteColumns(int position, int number) = 0;
};

/**
 * An R-Tree leaf.
 */
template<typename T>
class RTree<T>::LeafNode : public RTree<T>::Node, public KoRTree<T>::LeafNode
{
public:
  LeafNode(int capacity, int level, Node * parent)
  : KoRTree<T>::Node(capacity, level, parent)
  , RTree<T>::Node(capacity, level, parent)
  , KoRTree<T>::LeafNode(capacity, level, parent) {}
  virtual ~LeafNode() {}

  virtual void remove( const QRectF& rect, const T& data );
  virtual void contains( const QRectF& rect, QMap<int,T>& result ) const;
  virtual void intersectingPairs( const QRectF& rect, QMap<int,QPair<QRectF,T> >& result ) const;
  virtual void insertRows(int position, int number);
  virtual void insertColumns(int position, int number);
  virtual void deleteRows(int position, int number);
  virtual void deleteColumns(int position, int number);
};

/**
 * An R-Tree node.
 */
template<typename T>
class RTree<T>::NoneLeafNode : public RTree<T>::Node, public KoRTree<T>::NoneLeafNode
{
public:
  NoneLeafNode(int capacity, int level, Node * parent)
  : KoRTree<T>::Node(capacity, level, parent)
  , RTree<T>::Node(capacity, level, parent)
  , KoRTree<T>::NoneLeafNode(capacity, level, parent) {}
  virtual ~NoneLeafNode() {}

  virtual void remove( const QRectF& rect, const T& data );
  virtual void contains( const QRectF& rect, QMap<int,T>& result ) const;
  virtual void intersectingPairs( const QRectF& rect, QMap<int,QPair<QRectF,T> >& result ) const;
  virtual void insertRows(int position, int number);
  virtual void insertColumns(int position, int number);
  virtual void deleteRows(int position, int number);
  virtual void deleteColumns(int position, int number);
};


/////////////////////////////////////////////////////////////////////////////
// RTree definition
//
template<typename T>
RTree<T>::RTree()
  : KoRTree<T>(8, 4)
{
  delete this->m_root;
  this->m_root = new LeafNode( this->m_capacity + 1, 0, 0 );
}

template<typename T>
RTree<T>::~RTree()
{
}

template<typename T>
void RTree<T>::insert( const QRectF& rect, const T& data )
{
  KoRTree<T>::insert( rect.normalized().adjusted(0, 0, -0.1, -0.1), data );
}

template<typename T>
void RTree<T>::remove( const QRectF& rect, const T& data )
{
  dynamic_cast<Node*>(this->m_root)->remove( rect.normalized().adjusted(0, 0, -0.1, -0.1), data );
}

template<typename T>
QList<T> RTree<T>::contains( const QPointF& point ) const
{
  return KoRTree<T>::contains( point );
}

template<typename T>
QList<T> RTree<T>::contains( const QRectF& rect ) const
{
    QMap<int,T> result;
    dynamic_cast<Node*>(this->m_root)->contains( rect.normalized().adjusted(0, 0, -0.1, -0.1), result );
    return result.values();
}

template<typename T>
QList<T> RTree<T>::intersects( const QRectF& rect ) const
{
    return KoRTree<T>::intersects( rect.normalized().adjusted(0, 0, -0.1, -0.1) );
}

template<typename T>
QList< QPair<QRectF,T> > RTree<T>::intersectingPairs( const QRectF& rect ) const
{
    QMap<int,QPair<QRectF,T> > result;
    dynamic_cast<Node*>(this->m_root)->intersectingPairs( rect.normalized().adjusted(0, 0, -0.1, -0.1), result );
    return result.values();
}

template<typename T>
void RTree<T>::insertRows(int position, int number, InsertMode mode)
{
    if (position < 1 || position > KS_rowMax)
        return;
    const int pos = position - (mode == CopyPrevious) ? 1 : 0;
    dynamic_cast<Node*>(this->m_root)->insertRows(pos, number);
}

template<typename T>
void RTree<T>::insertColumns(int position, int number, InsertMode mode)
{
    if (position < 1 || position > KS_colMax)
        return;
    const int pos = position - (mode == CopyPrevious) ? 1 : 0;
    dynamic_cast<Node*>(this->m_root)->insertColumns(pos, number);
}

template<typename T>
void RTree<T>::deleteRows(int position, int number, InsertMode mode)
{
    if (position < 1 || position > KS_rowMax)
        return;
    const int pos = position - (mode == CopyPrevious) ? 1 : 0;
    dynamic_cast<Node*>(this->m_root)->deleteRows(pos, number);
}

template<typename T>
void RTree<T>::deleteColumns(int position, int number, InsertMode mode)
{
    if (position < 1 || position > KS_colMax)
        return;
    const int pos = position - (mode == CopyPrevious) ? 1 : 0;
    dynamic_cast<Node*>(this->m_root)->deleteColumns(pos, number);
}

/////////////////////////////////////////////////////////////////////////////
// RTree<T>::LeafNode definition
//
template<typename T>
void RTree<T>::LeafNode::remove( const QRectF& rect, const T& data )
{
    int old_counter = this->m_counter;
    for ( int i = 0; i < this->m_counter; ++i )
    {
        if ( this->m_childBoundingBox[i] == rect && this->m_data[i] == data )
        {
            //qDebug() << "LeafNode::remove id" << i;
            KoRTree<T>::LeafNode::remove( i );
            break;
        }
    }
    if ( old_counter == this->m_counter )
    {
        qWarning( "LeafNode::remove( const T&data) data not found" );
    }
}

template<typename T>
void RTree<T>::LeafNode::contains( const QRectF& rect, QMap<int,T>& result ) const
{
    for ( int i = 0; i < this->m_counter; ++i )
    {
        if ( this->m_childBoundingBox[i].contains( rect ) )
        {
            result.insert( this->m_dataIds[i], this->m_data[i] );
        }
    }
}

template<typename T>
void RTree<T>::LeafNode::intersectingPairs( const QRectF& rect, QMap<int,QPair<QRectF,T> >& result ) const
{
    for ( int i = 0; i < this->m_counter; ++i )
    {
        if ( this->m_childBoundingBox[i].intersects( rect ) )
        {
            QRectF rect = this->m_childBoundingBox[i].adjusted(0, 0, 0.1, 0.1);
            result.insert( this->m_dataIds[i], qMakePair(rect, this->m_data[i]) );
        }
    }
}

template<typename T>
void RTree<T>::LeafNode::insertRows(int position, int number)
{
  if (position > this->m_boundingBox.bottom())
  {
    return;
  }

  for ( int i = 0; i < this->childCount(); ++i )
  {
    this->m_childBoundingBox[i].adjust(0, (position < this->m_childBoundingBox[i].top()) ? number : 0, 0, number);
  }

  // position < m_rect.top() ? shift : extend
  this->m_boundingBox.adjust(0, (position < this->m_boundingBox.top()) ? number : 0, 0, number);
}

template<typename T>
void RTree<T>::LeafNode::insertColumns(int position, int number)
{
  if (position > this->m_boundingBox.right())
  {
    return;
  }

  for ( int i = 0; i < this->childCount(); ++i )
  {
    this->m_childBoundingBox[i].adjust((position < this->m_childBoundingBox[i].left()) ? number : 0, 0, number, 0);
  }

  // position < m_rect.left() ? shift : extend
  this->m_boundingBox.adjust((position < this->m_boundingBox.left()) ? number : 0, 0, number, 0);
}

template<typename T>
void RTree<T>::LeafNode::deleteRows(int position, int number)
{
  if (position > this->m_boundingBox.bottom())
  {
    return;
  }

  // position < m_rect.top() ? shift : extend
  this->m_boundingBox.adjust(0, (position < this->m_boundingBox.top()) ? -number : 0, 0, -number);
  if (this->m_boundingBox.isEmpty())
  {
      kDebug() << "bounding box is empty." << endl;
// ###    Node::erase();
    return;
  }

  for ( int i = 0; i < this->childCount(); ++i )
  {
    this->m_childBoundingBox[ i ].adjust(0, (position < this->m_childBoundingBox[ i ].top()) ? -number : 0, 0, -number);
    if (this->m_childBoundingBox[ i ].isEmpty())
    {
      KoRTree<T>::LeafNode::remove( i-- );
    }
  }
}

template<typename T>
void RTree<T>::LeafNode::deleteColumns(int position, int number)
{
  if (position > this->m_boundingBox.right())
  {
    return;
  }

  // position < m_rect.left() ? shift : extend
  this->m_boundingBox.adjust((position < this->m_boundingBox.left()) ? -number : 0, 0, -number, 0);
  if (this->m_boundingBox.isEmpty())
  {
      kDebug() << "bounding box is empty." << endl;
// ###    Node::erase();
    return;
  }

  for ( int i = 0; i < this->childCount(); ++i )
  {
    this->m_childBoundingBox[ i ].adjust((position < this->m_childBoundingBox[ i ].left()) ? -number : 0, 0, -number, 0);
    if (this->m_childBoundingBox[ i ].isEmpty())
    {
      KoRTree<T>::LeafNode::remove( i-- );
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
// RTree<T>::NoneLeafNode definition
//
template<typename T>
void RTree<T>::NoneLeafNode::remove( const QRectF& rect, const T& data )
{
    for ( int i = 0; i < this->m_counter; ++i )
    {
        if ( this->m_childBoundingBox[i].contains( rect ) )
        {
            dynamic_cast<Node*>(this->m_childs[i])->remove( rect, data );
        }
    }
}

template<typename T>
void RTree<T>::NoneLeafNode::contains( const QRectF& rect, QMap<int,T>& result ) const
{
    for ( int i = 0; i < this->m_counter; ++i )
    {
        if ( this->m_childBoundingBox[i].intersects( rect ) )
        {
            this->m_childs[i]->intersects( rect, result );
        }
    }
}

template<typename T>
void RTree<T>::NoneLeafNode::intersectingPairs( const QRectF& rect, QMap<int,QPair<QRectF,T> >& result ) const
{
    for ( int i = 0; i < this->m_counter; ++i )
    {
        if ( this->m_childBoundingBox[i].intersects( rect ) )
        {
           dynamic_cast<Node*>(this->m_childs[i])->intersectingPairs( rect, result );
        }
    }
}

template<typename T>
void RTree<T>::NoneLeafNode::insertRows(int position, int number)
{
  if (position > this->m_boundingBox.bottom())
    return;

  for ( int i = 0; i < this->childCount(); ++i )
  {
    this->m_childBoundingBox[i].adjust(0, (position < this->m_childBoundingBox[i].top()) ? number : 0, 0, number);
    dynamic_cast<Node*>(this->m_childs[i])->insertRows(position, number);
  }

  // position < m_rect.top() ? shift : extend
  this->m_boundingBox.adjust(0, (position < this->m_boundingBox.top()) ? number : 0, 0, number);
}

template<typename T>
void RTree<T>::NoneLeafNode::insertColumns(int position, int number)
{
  if (position > this->m_boundingBox.right())
  {
    return;
  }

  for ( int i = 0; i < this->childCount(); ++i )
  {
    this->m_childBoundingBox[i].adjust((position < this->m_childBoundingBox[i].left()) ? number : 0, 0, number, 0);
    dynamic_cast<Node*>(this->m_childs[i])->insertColumns(position, number);
  }

  // position < m_rect.left() ? shift : extend
  this->m_boundingBox.adjust((position < this->m_boundingBox.left()) ? number : 0, 0, number, 0);
}

template<typename T>
void RTree<T>::NoneLeafNode::deleteRows(int position, int number)
{
  if (position > this->m_boundingBox.bottom())
    return;

  // position < m_rect.top() ? shift : extend
  this->m_boundingBox.adjust(0, (position < this->m_boundingBox.top()) ? -number : 0, 0, -number);
  if (this->m_boundingBox.isEmpty())
  {
// ###    Node::erase();
    return;
  }

  for ( int i = 0; i < this->childCount(); ++i )
  {
    this->m_childBoundingBox[ i ].adjust(0, (position < this->m_childBoundingBox[ i ].top()) ? -number : 0, 0, -number);
    dynamic_cast<Node*>(this->m_childs[i])->deleteRows(position, number);
  }
}

template<typename T>
void RTree<T>::NoneLeafNode::deleteColumns(int position, int number)
{
  if (position > this->m_boundingBox.right())
    return;

  // position < m_rect.left() ? shift : extend
  this->m_boundingBox.adjust((position < this->m_boundingBox.left()) ? -number : 0, 0, -number, 0);
  if (this->m_boundingBox.isEmpty())
  {
// ###    Node::erase();
    return;
  }

  for ( int i = 0; i < this->childCount(); ++i )
  {
    this->m_childBoundingBox[ i ].adjust((position < this->m_childBoundingBox[ i ].left()) ? -number : 0, 0, -number, 0);
    dynamic_cast<Node*>(this->m_childs[i])->deleteColumns(position, number);
  }
}

} // namespace KSpread

#endif // KSPREAD_RTREE
