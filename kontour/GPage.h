/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998-1999 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2000-2002 Igor Janssen (rm@kde.org)

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

#ifndef __GPage_h__
#define __GPage_h__

#include <qobject.h>
#include <qptrlist.h>
#include <koRect.h>
#include <koPageLayoutDia.h>
#include "Handle.h"

class KoPainter;
class QDomDocument;
class QDomElement;
class KoColor;
namespace Kontour
{
class GDocument;
class GLayer;
class GObject;
class GPage : public QObject
{
  Q_OBJECT
public:
  GPage(GDocument *aGDoc);
  ~GPage();

  GDocument *document(void) const {return mGDoc; }

  QString name() const {return mName; }
  void name(QString aName);

  void bgColor(QColor aBGColor);
  QColor bgColor() const { return mBGColor; }

  int paperWidth() const {return mPaperWidth; }
  int paperHeight() const {return mPaperHeight; }
  void setPaperSize(int width, int height);

  KoPageLayout pageLayout();
  void pageLayout(const KoPageLayout &layout);

  Handle &handle() {return mHandle; }

  QDomElement saveToXml(QDomDocument &doc);
  bool readFromXml(const QDomElement &page);

  /**
   * Layers management.
   *
   */

  QPtrList<GLayer> &getLayers() {return layers; }

  // retrieve the active layer
  GLayer *activeLayer();

  // set the active layer where further actions take place
  void activeLayer(GLayer *aLayer);

  // add a new layer on top of existing layers
  GLayer *addLayer();

  // delete the given layer as well as all contained objects
  void deleteLayer(GLayer *aLayer);

  // raise the given layer
  void raiseLayer(GLayer *aLayer);

  // lower the given layer
  void lowerLayer(GLayer *aLayer);

  /**
   *  Objects management,
   *
   */

  unsigned int objectCount() const;

  void insertObject(GObject *obj);
  void deleteObject(GObject *obj);

  unsigned int findIndexOfObject(GObject *obj);
  void insertObjectAtIndex(GObject *obj, unsigned int idx);
  void moveObjectToIndex(GObject *obj, unsigned int idx);

  void selectObject(GObject *obj);
  void unselectObject(GObject *obj);

  void selectAllObjects();
  void unselectAllObjects();

  void selectNextObject();

  void deleteSelectedObjects();

  const KoRect &boundingBoxForSelection() const {return mBBox; }
  const KoRect &shapeBoxForSelection() const {return mSBox; }
  void calcBoxes();

  QPtrList<GObject> &getSelection() {return selection; }
  bool selectionIsEmpty() const {return selection.isEmpty(); }
  unsigned int selectionCount() const {return selection.count(); }
  unsigned int convertibleCount() const {return mConvertibleCount; }

  void drawContents(KoPainter *p, const QWMatrix &m, bool withBasePoints = false, bool outline = false, bool withEditMarks = true);

  GObject *findContainingObject(double x, double y);
  bool findContainingObjects(int x, int y, QPtrList<GObject> &olist);
  bool findObjectsContainedIn(const KoRect &r, QPtrList<GObject> &olist);

  void updateSelection();

private:
  GDocument                  *mGDoc;

  QString                     mName;                        // page name
  QColor                      mBGColor;                     // background color
  int                         mPaperWidth;                  // paper width (pt)
  int                         mPaperHeight;                 // paper height (pt)

  QPtrList<GLayer>            layers;                       // the array of all layers
  GLayer                     *mActiveLayer;                 // the current layer
  QPtrList<GObject>           selection;                    // the array of selected objects

  int                         mCurLayerNum;
  unsigned int                mConvertibleCount;

  KoPageLayout                mPageLayout;

  KoRect                      mBBox;
  KoRect                      mSBox;
  Handle                      mHandle;
};
};
using namespace Kontour;

#endif
