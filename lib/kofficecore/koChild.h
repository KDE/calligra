/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef __koChild_h__
#define __koChild_h__

#include <qobject.h>
#include <qwmatrix.h>

/**
 * KoChild is an abstract base class that represents the geometry
 * associated with an embedded document. In general it handles its position
 * relative to the embedded documents parent.
 *
 * In detail it handles size, matrix operations and can give you
 * a clip region. It can deal with scaling, rotation etc. because it
 * makes heavy usage of QWMatrix.
 *
 * @see KoDocumentChild
 */
class KoChild : public QObject
{
  Q_OBJECT
public:
  enum Gadget { NoGadget, TopLeft, TopMid, TopRight, MidLeft, MidRight,
		BottomLeft, BottomMid, BottomRight, Move };

  KoChild( QObject *parent = 0, const char *name = 0 );
  virtual ~KoChild();

  /**
   *  Sets a new geometry for this child document.
   *  Use noEmit = true if you do not want the 'changed'-signal to be emitted
   */
  virtual void setGeometry( const QRect &rect, bool noEmit = false );

  /**
   * @return the rectangle that would be used to display this
   *         child document if the child is not rotated or
   *         subject to some other geometric transformation.
   *         The rectangle is in the coordinate system of the parent.
   *
   * @see #setGeometry
   */
  virtual QRect geometry() const;

  /**
   * @return the region of this child part relative to the
   *         coordinate system of the parent.
   *         The region is transformed with the passed
   *         matrix.
   */
  virtual QRegion region( const QWMatrix& = QWMatrix() ) const;

  /**
   * @return the polygon which surrounds the child part. The points
   *         are in coordinates of the parent.
   *         The points are transformed with the
   *         passed matrix.
   */
  virtual QPointArray pointArray( const QWMatrix &matrix = QWMatrix() ) const;

  /**
   * Tests whether the part contains a certain point. The point is
   * in the coordinate system of the parent.
   */
  virtual bool contains( const QPoint& ) const;

  /**
   * @return the effective bounding rect after all transformations.
   *         The coordinates of the rectangle are in the coordinate system
   *         of the parent.
   */
  virtual QRect boundingRect( const QWMatrix &matrix = QWMatrix() ) const;

  /**
   * Scales the content of the child part. However, that does not
   * affect the size of the child part.
   */
  virtual void setScaling( double x, double y );

  /**
   * @return the x axis scaling of the child part
   */
  virtual double xScaling() const;

  /**
   * @return the y axis scaling of the child part
   */
  virtual double yScaling() const;

  /**
   * Shears the content of the child part.
   */
  virtual void setShearing( double x, double y );

  /**
   * @return the x axis shearing of the child part
   */
  virtual double xShearing() const;

  /**
   * @return the y axis shearing of the child part
   */
  virtual double yShearing() const;

  /**
   * Sets the angle of rotation.
   */
  virtual void setRotation( double );

  /**
   * @return the angle of rotation
   */
  virtual double rotation() const;

  /**
   * Sets the center of the rotation to the point @p pos.
   */
  virtual void setRotationPoint( const QPoint& pos );

  /**
   * @return the center of the rotation
   */
  virtual QPoint rotationPoint() const;

  /**
   * @return true if the child part is an orthogonal rectangle relative
   *         to its parents coordinate system.
   */
  virtual bool isRectangle() const;

  /**
   * Sets the clip region of the painter, so that only pixels of the
   * child part can be drawn.
   *
   * @param combine tells whether the new clip region is an intersection
   *        of the current region with the childs region or whether only
   *        the childs region is set.
   */
  virtual void setClipRegion( QPainter& painter, bool combine = true );

  /**
   * Transforms the painter (its worldmatrix and the clipping)
   * in such a way that the painter can be passed to the child part
   * for drawing.
   */
  virtual void transform( QPainter& painter );

  /// @todo ### make virtual
  void setContentsPos( int x, int y );

  /**
   * @return the contents rectangle that is visible.
   *         This value depends on the scaling and the
   *         geometry.
   *
   * @see #xScaling #geometry
   */
  virtual QRect contentRect() const;

  virtual QRegion frameRegion( const QWMatrix& matrix, bool solid = false ) const;

  virtual QPointArray framePointArray( const QWMatrix &matrix = QWMatrix() ) const;

  virtual QWMatrix matrix() const;

  virtual void lock();

  virtual void unlock();

  bool locked() const;

  virtual QPointArray oldPointArray( const QWMatrix &matrix );

  virtual void setTransparent( bool transparent );

  virtual bool isTransparent() const;

  virtual Gadget gadgetHitTest( const QPoint& p, const QWMatrix& matrix );

signals:
  void changed( KoChild *thisChild );

protected:

  virtual QPointArray pointArray( const QRect& r, const QWMatrix& matrix ) const;

  virtual void updateMatrix();
private:

  class KoChildPrivate;
  KoChildPrivate *d;
};

#endif
