/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2000-01 Igor Janssen (rm@linux.ru.net)

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

#ifndef GPage_h_
#define GPage_h_

#include <qobject.h>
#include <qlist.h>
#include <qvaluelist.h>
#include <qstring.h>

#include <Handle.h>

#include <koPageLayoutDia.h>

#define KILLUSTRATOR_MIMETYPE "application/x-killustrator"

class GDocument;
class GObject;
class GLayer;
class QDomDocument;
class QDomElement;

class GPage : public QObject
 {
  Q_OBJECT
public:
  GPage (GDocument *doc);
  ~GPage ();

  void initialize ();
  void setAutoUpdate (bool flag);

  void setPaperSize (int width, int height);
  int getPaperWidth () const;
  int getPaperHeight () const;

  void drawContents (QPainter& p, bool withBasePoints = false,
                     bool outline = false);
  void drawContentsInRegion (QPainter& p, const Rect& r, const Rect& rr,
                             bool withBasePoints = false,
                             bool outline = false);

  void invalidateClipRegions ();

  /*
   * Layer management
   */

  // get an array with all layers of the document
  const QList<GLayer>& getLayers ();

  // set the active layer where further actions take place
  void setActiveLayer (GLayer *layer);

  // retrieve the active layer
  GLayer* activeLayer ();

  // raise the given layer
  void raiseLayer (GLayer *layer);

  // lower the given layer
  void lowerLayer (GLayer *layer);

  // add a new layer on top of existing layers
  GLayer* addLayer ();

  // delete the given layer as well as all contained objects
  void deleteLayer (GLayer *layer);

  // return helpline layer
  GLayer* layerForHelplines ();
  bool helplineLayerIsActive ();

  void insertObject (GObject* obj);
  void selectObject (GObject* obj);
  void unselectObject (GObject* obj);
  void unselectAllObjects ();
  void selectAllObjects ();

  void selectNextObject ();
  void selectPrevObject ();

  GObject* lastObject () { return last; }
  void setLastObject (GObject* obj);

  QList<GObject>& getSelection () { return selection; }
  bool selectionIsEmpty () const { return selection.isEmpty (); }
  unsigned int selectionCount () const { return selection.count(); }

  unsigned int objectCount () const;

  Rect boundingBoxForSelection ();
  Rect boundingBoxForAllObjects ();
  void deleteSelectedObjects ();
  void deleteObject (GObject* obj);

  GObject* findContainingObject (int x, int y);

  bool findNearestObject (const QString &otype, int x, int y,
                          float max_dist, GObject*& obj, int& pidx,
                          bool all = false);

  bool findContainingObjects (int x, int y, QList<GObject>& olist);
  bool findObjectsContainedIn (const Rect& r, QList<GObject>& olist);
  
/*
   Load/Save Page.
*/
  QDomDocument saveToXml();
  bool readFromXml (const QDomDocument &document);
  bool insertFromXml (const QDomDocument &document, QList<GObject>& newObjs);

  Handle& handle () { return selHandle; }

  unsigned int findIndexOfObject (GObject *obj);
  void insertObjectAtIndex (GObject* obj, unsigned int idx);
  void moveObjectToIndex (GObject* obj, unsigned int idx);

  KoPageLayout pageLayout ();
  void setPageLayout (const KoPageLayout& layout);

protected:
  void updateHandle ();
  bool parseBody (const QDomElement &element, QList<GObject>& newObjs, bool markNew);

public slots:
  void objectChanged ();
  void objectChanged (const Rect& r);
  void layerChanged ();

signals:
  void handleChanged();
  void changed ();
  void changed (const Rect& r);
  void selectionChanged ();
  void sizeChanged ();

  void wasModified (bool flag);

protected:
  bool autoUpdate;
  QString name;  // ?
  int paperWidth, paperHeight; // pt
  QList<GLayer> layers; // the array of all layers
  QList<GObject> selection;
  GLayer* active_layer;     // the current layer
  GObject *last;
  Handle selHandle;
  Rect selBox;
  bool selBoxIsValid;
  KoPageLayout pLayout;
};

#endif
