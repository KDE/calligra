/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __GObject_h__
#define __GObject_h__

#include <qobject.h>
#include <qwmatrix.h>

#include <koPoint.h>
#include <koRect.h>

#include "GStyle.h"

class QDomDocument;
class QDomElement;
class KoPainter;
namespace Kontour
{
class GPath;
class GLayer;
class GOState;

/**
 * The base class for all graphical objects.
 *
 * Documentation
 *
 * @author  Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
 * @version $Revision$
 */
class GObject : public QObject
{
  Q_OBJECT
protected:

  /**
   *
   *
   */
  GObject();
  
  /**
   *
   *
   */
  GObject(const QDomElement &element);
  
  /**
   *
   *
   */
  GObject(const GObject &obj);

public:
  
  /**
   * The destructor.
   *
   */
  virtual ~GObject();

  /**
   *
   */
  virtual GObject *copy() const = 0;

  unsigned int id() const {return mId; }

  void ref();
  void unref();
  
  /**
   *
   *
   */
  virtual QString typeName() const = 0;
  
  /**
   *
   *
   */
  GLayer *layer() const {return mLayer; }
  void layer(GLayer *l);
  
  /**
   * Mark the object as selected or not selected.
   *
   * @param flag if true, the object is selected, otherwise the object will
   *             be unselected.
   */
  virtual void select(bool flag = true);

  /**
   * Retrieve the selection status of the object.
   *
   * @return true, if the object is selected, otherwise false.
   */
  bool isSelected() const {return sflag; }
  
  bool workInProgress() const {return inWork; }
  void setWorkInProgress(bool flag);
  
  /**
   *
   *
   */
  virtual QDomElement writeToXml(QDomDocument &document);

  /**
   * Draw the object with the given painter.
   * NOTE: This method has to be implemented in every subclass.
   *
   * @param p The Painter for drawing the object.
   * @param withBasePoints If true, draw the base points of the
   *                       object.
   */
  virtual void draw(KoPainter *p, const QWMatrix &m, bool withBasePoints = false, bool outline = false, bool withEditMarks = true) = 0;

  void drawNode(KoPainter *p, int x, int y, bool active);
  
  /**
   * Change all coords of object for new zoom factor.
   *
   * @param f new zoom factor
   * @param pf previous zoom factor
   */
  virtual void setZoomFactor(double f, double pf);
 
  /*
   * Style management
   */
  GStyle *style() const {return mStyle; }
  void style(const GStyle *s);

  /**
   * Control points.
   *
   */
  virtual int getNeighbourPoint(const KoPoint &point, const double distance) = 0;
  virtual void movePoint(int idx, double dx, double dy, bool ctrlPressed = false) = 0;
  virtual void removePoint(int idx) = 0;

  virtual void calcBoundingBox() = 0;

  /**
   * Retrieve the transformation matrix associated with the object.
   *
   * @return The current matrix.
   */
  const QWMatrix &matrix() const {return tMatrix; }

  void matrix(QWMatrix m);

  /**
   * Initialize a temporary matrix for transformation from the values of
   * the transformation matrix. The temporary matrix is used by interactive
   * tools like the SelectionTool.
   */
  void initTmpMatrix();

  /**
   * Transform the object according to the given matrix.
   */
  void transform(const QWMatrix &m);

  /**
   * Transform the object temporary according to the given matrix. The
   * transformation matrix is not modified.
   */
  void ttransform(const QWMatrix &m);
  
  /**
   * Test, if the object contains the given point.
   * NOTE: This method has to be implemented in every subclass.
   *
   * @param p  The coodinates of a point.
   * @return   true, if the object contains the point, otherwise false.
   */
  virtual bool contains(const KoPoint &p) = 0;
  
  const KoRect &boundingBox() const {return mBBox; }
  const KoRect &shapeBox() const {return mSBox; }

  /**
   * Convert object to path.
   * 
   * @return pointer to new path
   */
  virtual GPath *convertToPath() const = 0;
  virtual bool isConvertible() const = 0;

  static GObject *objectFactory(const QDomElement &element);

signals:
  void deleted();
  void changed();
  void changed(const KoRect &r);

protected:
  QDomElement createMatrixElement(const QWMatrix &matrix, QDomDocument &document);
  QWMatrix toMatrix(const QDomElement &matrix);
  KoRect calcUntransformedBoundingBox(const KoPoint &tleft, const KoPoint &tright, const KoPoint &bright, const KoPoint &bleft);
  void setPen(KoPainter *p);
  void setBrush(KoPainter *p);

  /* Adjust the bounding box by taking into account linewidth */
  void adjustBBox(KoRect &rect);

protected:
  unsigned int rcount;            // the reference counter
  unsigned int mId;               // object ID

  GLayer *mLayer;                 // the layer containing this object

  bool sflag:1;                   // object is selected
  bool inWork:1;                  // the object is currently manipulated,

  GStyle *mStyle;

  KoRect mBBox;                   // the bounding box
  KoRect mSBox;

  QWMatrix tMatrix;               // transformation matrix
  QWMatrix tmpMatrix;             // temporary transformation matrix
  QWMatrix iMatrix;               // inverted transformation matrix

  static unsigned int mCurId;     // ID for new objects
};
};
using namespace Kontour;

#endif
