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

#include <qfile.h>

#include "GPage.h"
#include "GDocument.h"
#include "GPolygon.h"
#include "GText.h"
#include "GPolyline.h"
#include "GOval.h"
#include "GBezier.h"
#include "GClipart.h"
#include "GGroup.h"
#include "GPixmap.h"
#include "GCurve.h"
#include "GLayer.h"

#include <assert.h>
#include <kdebug.h>

#ifdef __FreeBSD__
#include <math.h>
#else
#include <values.h>
#endif

#include <float.h>

#include <units.h>
#include <klocale.h>
#include <qdict.h>
#include <qdom.h>

#define LAYER_VISIBLE   1
#define LAYER_EDITABLE  2
#define LAYER_PRINTABLE 4

#define KILLUSTRATOR_MIMETYPE "application/x-killustrator"

GPage::GPage (GDocument *adoc):
doc(adoc),selHandle(adoc)
{
  connect(this, SIGNAL(changed(const Rect&)),doc, SLOT(emitChanged(const Rect&)));
  connect(this, SIGNAL(changed()),doc, SLOT(emitChanged()));
  connect(this, SIGNAL(handleChanged()),doc, SLOT(emitHandleChanged()));
  initialize ();
}

GPage::~GPage ()
{
  layers.clear();
  selection.clear();
}

void GPage::initialize ()
{
  pLayout.format = PG_DIN_A4;
  pLayout.orientation = PG_PORTRAIT;
  pLayout.mmWidth = PG_A4_WIDTH;
  pLayout.mmHeight = PG_A4_HEIGHT;
  pLayout.mmLeft = 0; pLayout.mmRight = 0;
  pLayout.mmTop = 0; pLayout.mmBottom = 0;
  pLayout.unit = PG_MM;

  // in pt !!
  paperWidth = (int) cvtMmToPt (pLayout.mmWidth);
  paperHeight = (int) cvtMmToPt (pLayout.mmHeight);
  last = 0L;
  filename = i18n("<unnamed>");

  selection.clear ();
  layers.setAutoDelete(true);
  layers.clear ();
  // add layer for Helplines
  GLayer *l = addLayer ();
  l->setInternal ();
  l->setName (i18n("Helplines"));
  connect (l, SIGNAL(propertyChanged ()),
           this, SLOT(helplineStatusChanged ()));
  active_layer = addLayer ();
  active_layer->setVisible(true);
  active_layer->setPrintable(true);
  active_layer->setEditable(true);

  selBoxIsValid = false;
  autoUpdate = true;
  emit changed ();
}

void GPage::setPaperSize (int width, int height)
{
  paperWidth = width;
  paperHeight = height;
}

int GPage::getPaperWidth () const
{
  return paperWidth;
}

int GPage::getPaperHeight () const
{
  return paperHeight;
}

void GPage::setName(QString aName)
{
 mName = aName;
}

void GPage::emitHandleChanged()
{
  emit handleChanged();
}

void GPage::drawContents (QPainter& p, bool withBasePoints, bool outline)
{
  for (QListIterator<GLayer> i(layers); i.current(); ++i)
  {
    if (! (*i)->isInternal () && (*i)->isVisible ())
    {
      const QList<GObject> &contents = (*i)->objects ();
      QListIterator<GObject> oi(contents);
      for ( ; oi.current() ; ++oi)
        (*oi)->draw (p, withBasePoints && (*oi)->isSelected (), outline);
    }
  }
}

void GPage::drawContentsInRegion (QPainter& p, const Rect& r, const Rect& rr,
                                      bool withBasePoints, bool outline)
{
  for (QListIterator<GLayer> i(layers); i.current(); ++i)
  {
    if (! (*i)->isInternal () && (*i)->isVisible ())
    {
      const QList<GObject> &contents = (*i)->objects ();
      QListIterator<GObject> oi(contents);
      for ( ; oi.current() ; ++oi)
      {
            // draw the object only if its bounding box
            // intersects the active region
            //      const Rect& bbox = (*oi)->boundingBox ();
            //      if (r.intersects (bbox))
        if ((*oi)->intersects (rr))
          (*oi)->draw (p, withBasePoints && (*oi)->isSelected (), outline);
      }
    }
  }
}

unsigned int GPage::objectCount () const
{
  unsigned int num = 0;
  for (QListIterator<GLayer> i(layers); i.current(); ++i)
    num += (*i)->objectCount ();
  return num;
}

void GPage::insertObject (GObject* obj)
{

    obj->ref ();
    active_layer->insertObject (obj);
    connect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
    connect (obj, SIGNAL(changed(const Rect&)),
             this, SLOT(objectChanged (const Rect&)));
//    setModified ();
    if (autoUpdate)
        emit changed ();
}

void GPage::selectObject (GObject* obj)
{
  if (selection.containsRef(obj)==0)
  {
    // object isn't yet in selection list
    obj->select (true);
    selection.append(obj);
    selBoxIsValid = false;
    updateHandle ();
    if (autoUpdate)
    {
      emit changed ();
      emit selectionChanged ();
    }
  }
}

void GPage::unselectObject (GObject* obj)
{
  int i=selection.findRef(obj);
  if (i != -1)
  {
    // remove object from the selection list
    obj->select (false);
    selection.remove(i);
    selBoxIsValid = false;
    updateHandle ();
    if (autoUpdate)
    {
      emit changed ();
      emit selectionChanged ();
    }
  }
}

void GPage::unselectAllObjects ()
{
  if (selection.isEmpty())
    return;

  for(GObject *o=selection.first(); o!=0L; o=selection.next())
      o->select(false);
  selection.clear ();
  selBoxIsValid = false;
  if (autoUpdate)
  {
    emit changed ();
    emit selectionChanged ();
  }
}

void GPage::selectAllObjects ()
{
  selection.clear ();
  for (QListIterator<GLayer> i(layers); i.current(); ++i)
  {
    if ((*i)->isEditable ())
    {
        const QList<GObject> &contents = (*i)->objects ();
        QListIterator<GObject> oi(contents);
        for ( ; oi.current() ; ++oi)
	{
            GObject* obj = *oi;
            obj->select (true);
            selection.append(obj);
        }
    }
  }
  selBoxIsValid = false;
  updateHandle ();
  if (autoUpdate)
  {
    emit changed ();
    emit selectionChanged ();
  }
}

void GPage::setLastObject (GObject* obj)
{
  if (obj == 0L || obj->getLayer () != 0L)
    last = obj;
}


void GPage::updateHandle ()
{
  Rect r = boundingBoxForSelection ();
  if (selectionIsEmpty ())
    selHandle.show (false);
  else
    selHandle.setBox (r);
}


Rect GPage::boundingBoxForSelection ()
{
  if (! selBoxIsValid)
  {
    if (! selectionIsEmpty ())
    {
      QListIterator<GObject> i(selection);
      selBox = (*i)->boundingBox ();
      ++i;
      for (; i.current(); ++i)
        selBox = selBox.unite ((*i)->boundingBox ());
    }
    else
    {
      selBox = Rect ();
    }
    selBoxIsValid = true;
  }
  return selBox;
}

Rect GPage::boundingBoxForAllObjects ()
{
  Rect box;

  bool init = false;

  for (QListIterator<GLayer> li(layers); li.current(); ++li)
  {
    if (! (*li)->isInternal () && (*li)->isEditable ())
    {
      const QList<GObject> &contents = (*li)->objects ();
      QListIterator<GObject> oi(contents);
      if (! init)
      {
        box = (*oi)->boundingBox ();
        ++oi;
        init = true;
      }
      for (; oi.current(); ++oi)
        box = box.unite ((*oi)->boundingBox ());
    }
  }
  return box;
}

void GPage::deleteSelectedObjects ()
{
  if (! selectionIsEmpty ())
  {
    for(QListIterator<GObject> i(selection); i.current(); ++i)
    {
      GObject* obj = *i;
      disconnect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
      disconnect (obj, SIGNAL(changed(const Rect&)),
                  this, SLOT(objectChanged (const Rect&)));
      obj->getLayer ()->deleteObject (obj);
    }
    selection.clear ();
    last = 0L;
//    setModified ();
    selBoxIsValid = false;
    if (autoUpdate)
    {
      emit changed ();
      emit selectionChanged ();
    }
  }
}

void GPage::deleteObject (GObject* obj)
{
  bool selected = false;

  GLayer* layer = obj->getLayer ();
  assert (layer);
  if (layer->isEditable ())
  {
    selected = obj->isSelected ();
    if (selected)
      selection.removeRef(obj);
    last = 0L;
//    setModified ();
    disconnect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
    disconnect (obj, SIGNAL(changed(const Rect&)),
                this, SLOT(objectChanged (const Rect&)));
    layer->deleteObject (obj);
    if (selected)
    {
      selBoxIsValid = false;
      updateHandle ();
      if (autoUpdate)
        emit selectionChanged ();
    }
    if (autoUpdate)
      emit changed ();
  }
}

/**
 * Looks for an object of type <tt>otype</tt> which endpoints are distant
 * not more than <tt>max_dist</tt> from the point <tt>x, y</tt>.
 * The method returns <tt>true</tt> if an object was found as well as
 * the object in <tt>obj</tt> and the index of the nearest point in
 * <tt>pidx</tt>.
 */
bool GPage::findNearestObject (const QString &otype, int x, int y,
                                   float max_dist, GObject*& obj,
                                   int& pidx, bool all)
{
  float d, distance = FLT_MAX;
  obj = 0L;
  Coord p (x, y);

  QListIterator<GLayer> li(layers);
  for (li.toLast(); li.current(); --li)
  {
    if ((*li)->isEditable ())
    {
      const QList<GObject>& contents = (*li)->objects ();
      QListIterator<GObject> oi(contents);
      for ( ; oi.current(); ++oi)
      {
        if (otype == 0L || (*oi)->isA(otype.latin1()))
	{
          if ((*oi)->findNearestPoint (p, max_dist, d, pidx, all) && d < distance)
	  {
            obj = *oi;
            distance = d;
          }
        }
      }
    }
  }
  if (obj == 0L)
    pidx = -1;
  return obj != 0L;
}

GObject* GPage::findContainingObject (int x, int y)
{
  GObject* result = 0L;
  // We are looking for the most relevant object, that means the object
  // in front of all others. So, we have to start at the upper layer
  QListIterator<GLayer> i(layers);
  for (i.toLast(); i.current(); --i)
  {
    if ((*i)->isEditable ())
    {
      result = (*i)->findContainingObject (x, y);
      if (result)
        break;
    }
  }
  return result;
}

bool GPage::findContainingObjects (int x, int y, QList<GObject>& olist)
{
  Coord coord (x, y);
  for (QListIterator<GLayer> li(layers); li.current(); ++li)
  {
    if ((*li)->isEditable ())
    {
      const QList<GObject>& contents = (*li)->objects ();
      QListIterator<GObject> oi(contents);
      for ( ; oi.current(); ++oi)
        if ((*oi)->contains (coord))
          olist.append (*oi);
    }
  }
  return olist.count () > 0;
}

bool GPage::findObjectsContainedIn (const Rect& r, QList<GObject>& olist)
{
  for (QListIterator<GLayer> li(layers); li.current(); ++li)
  {
    if ((*li)->isEditable ())
    {
      const QList<GObject>& contents = (*li)->objects ();
      QListIterator<GObject> oi(contents);
      for ( ; oi.current(); ++oi)
        if (r.contains ((*oi)->boundingBox ()))
          olist.append (*oi);
    }
  }
  return olist.count () > 0;
}

void GPage::layerChanged ()
{
  if (!autoUpdate)
    return;
  emit changed ();
}

void GPage::objectChanged ()
{
  if (!autoUpdate)
    return;

  if (! selectionIsEmpty ())
  {
    selBoxIsValid = false;
    updateHandle ();
    GObject* obj = (GObject *) sender ();
    if (obj->isSelected () && autoUpdate)
    {
      emit selectionChanged ();
    }
  }
//  setModified ();
  if (autoUpdate)
    emit changed ();
}

void GPage::objectChanged (const Rect& r)
{
  if (!autoUpdate)
    return;

  if (! selectionIsEmpty ())
  {
    selBoxIsValid = false;
    updateHandle ();
    /*
    GObject* obj = (GObject *) sender ();
    if (obj->isSelected () && autoUpdate) {
      emit selectionChanged ();
    }
    */
  }
//  setModified ();
  if (autoUpdate)
    emit changed (r);
}

QDomElement GPage::saveToXml (QDomDocument &document)
{
  static const char* formats[] = {
    "a3", "a4", "a5", "us_letter", "us_legal", "screen", "custom"};
  static const char* orientations[] = {"portrait", "landscape"};

  kdDebug(0) << "Page saving...\n";

  QDomElement page = document.createElement("page");;

  page.setAttribute ("id", name ());

  QDomElement layout = document.createElement("layout");
  layout.setAttribute ("format", formats[pLayout.format]);
  layout.setAttribute ("orientation", orientations[pLayout.orientation]);
  layout.setAttribute ("width", pLayout.mmWidth);
  layout.setAttribute ("height", pLayout.mmHeight);
  layout.setAttribute ("lmargin", pLayout.mmLeft);
  layout.setAttribute ("tmargin", pLayout.mmTop);
  layout.setAttribute ("rmargin", pLayout.mmRight);
  layout.setAttribute ("bmargin", pLayout.mmBottom);
  page.appendChild(layout);

  bool save_layer_info = (layers.count() > 2);
  for (QListIterator<GLayer> li(layers); li.current(); ++li)
  {
    GLayer *l=(*li);
    if (l->isInternal ())
      continue;

    QDomElement layer;
    layer=document.createElement("layer");
    if(save_layer_info)
    {
      int flags = (l->isVisible () ? LAYER_VISIBLE : 0) +
              (l->isPrintable () ? LAYER_PRINTABLE : 0) +
                (l->isEditable () ? LAYER_EDITABLE : 0);
      layer.setAttribute ("id", l->name ());
      layer.setAttribute ("flags", QString::number(flags));
    }
    const QList<GObject>& contents = l->objects ();
    for (QListIterator<GObject> oi(contents);oi.current(); ++oi)
      layer.appendChild((*oi)->writeToXml (document));
    page.appendChild(layer);
  }
//  setModified (false);
  return page;
}

bool GPage::insertFromXml (const QDomDocument &document, QList<GObject>& newObjs)
{
  if ( document.doctype().name() != "killustrator" )
    return false;
  QDomElement doc = document.documentElement();

  if ( doc.attribute( "mime" ) != KILLUSTRATOR_MIMETYPE )
    return false;
  return parseBody (doc, newObjs, true);
}

bool GPage::parseBody (const QDomElement &element, QList<GObject>& /*newObjs*/, bool /*markNew*/)
{
  GObject* obj = 0L;
  QDict<GObject> refDict;

  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement layerelem=n.toElement();
    if (layerelem.tagName() == "layer")
    {
      QString id=layerelem.attribute("id");
      if(!id.isEmpty())
      {
        active_layer = addLayer ();
        active_layer->setName (id);
        int flags = layerelem.attribute("flags").toInt();
        active_layer->setVisible (flags & LAYER_VISIBLE);
        active_layer->setPrintable (flags & LAYER_EDITABLE);
        active_layer->setEditable (flags & LAYER_PRINTABLE);
      }

      QDomNode cn=layerelem.firstChild();
      while(!cn.isNull())
      {
        QDomElement child=cn.toElement();
        obj=KIllustrator::objectFactory(child, document()->document());
        if(!obj)
//                {
//                   GObject *proto = GObject::lookupPrototype (child.tagName());
//                   if (proto != 0L)
//                   {
//                      cout<<"********** calling lookupPrototype"<<endl;
//                      obj = proto->create (this, child);
//                   }
//                   else
                      kdDebug(38000) << "invalid object type: " << child.tagName() << endl;
//                }
        if (child.tagName() == "group")
          ((GGroup*)obj)->setLayer (active_layer);
        if(obj->hasId())
          refDict.insert(obj->getId(), obj);
        insertObject(obj);
        cn=cn.nextSibling();
      }
    }
    n=n.nextSibling();
  }

// update object connections
  for (QListIterator<GLayer> i(layers); i.current(); ++i)
  {
    const QList<GObject>& contents = (*i)->objects ();
    for (QListIterator<GObject> oi(contents); oi.current(); ++oi)
    {
    // this should be more general !!
      if ((*oi)->hasRefId () && (*oi)->isA ("GText"))
      {
        GObject *o = refDict[(*oi)->getRefId ()];
        if(o)
	{
          GText *tobj = (GText *) *oi;
          tobj->setPathObject (o);
        }
      }
    }
  }
  setAutoUpdate (true);
  return true;
}

bool GPage::readFromXml (const QDomElement &page)
{
  setAutoUpdate (false);

  setName(page.attribute("id"));
  QDomElement layout=page.namedItem("layout").toElement();
  QString tmp=layout.attribute("format");
  if (tmp == "a3")
    pLayout.format = PG_DIN_A3;
  else
    if (tmp == "a4")
      pLayout.format = PG_DIN_A4;
    else
      if (tmp == "a5")
        pLayout.format = PG_DIN_A5;
      else
      if (tmp == "usletter")
        pLayout.format = PG_US_LETTER;
      else
        if (tmp == "uslegal")
          pLayout.format = PG_US_LEGAL;
        else
	  if (tmp == "custom")
            pLayout.format = PG_CUSTOM;
          else
            pLayout.format = PG_DIN_A4;

  tmp=layout.attribute("orientation");
  if (tmp == "portrait")
    pLayout.orientation = PG_PORTRAIT;
  else
    if (tmp == "landscape")
      pLayout.orientation = PG_LANDSCAPE;
    else
      pLayout.orientation = PG_PORTRAIT;

  pLayout.mmWidth=layout.attribute("width").toFloat();
  pLayout.mmHeight=layout.attribute("height").toFloat();
  pLayout.mmLeft=layout.attribute("lmargin").toFloat();
  pLayout.mmRight=layout.attribute("rmargin").toFloat();
  pLayout.mmBottom=layout.attribute("bmargin").toFloat();
  pLayout.mmTop=layout.attribute("tmargin").toFloat();
 
// update page layout
  setPageLayout (pLayout);

  QList<GObject> dummy;
  bool result = parseBody (page, dummy, false);

//  setModified (false);
  return result;
}

bool GPage::readFromXmlV2 (const QDomElement &page)
{
  setAutoUpdate (false);
  QDomElement head=page.namedItem("head").toElement();
  
  QDomElement layout=head.namedItem("layout").toElement();
  QString tmp=layout.attribute("format");
  if (tmp == "a3")
    pLayout.format = PG_DIN_A3;
  else
    if (tmp == "a4")
      pLayout.format = PG_DIN_A4;
    else
      if (tmp == "a5")
        pLayout.format = PG_DIN_A5;
      else
      if (tmp == "usletter")
        pLayout.format = PG_US_LETTER;
      else
        if (tmp == "uslegal")
          pLayout.format = PG_US_LEGAL;
        else
	  if (tmp == "custom")
            pLayout.format = PG_CUSTOM;
          else
            pLayout.format = PG_DIN_A4;

  tmp=layout.attribute("orientation");
  if (tmp == "portrait")
    pLayout.orientation = PG_PORTRAIT;
  else
    if (tmp == "landscape")
      pLayout.orientation = PG_LANDSCAPE;
    else
      pLayout.orientation = PG_PORTRAIT;

  pLayout.mmWidth=layout.attribute("width").toFloat();
  pLayout.mmHeight=layout.attribute("height").toFloat();
  pLayout.mmLeft=layout.attribute("lmargin").toFloat();
  pLayout.mmRight=layout.attribute("rmargin").toFloat();
  pLayout.mmBottom=layout.attribute("bmargin").toFloat();
  pLayout.mmTop=layout.attribute("tmargin").toFloat();
 
// update page layout
  setPageLayout (pLayout);

  QList<GObject> dummy;
  bool result = parseBody (page, dummy, false);

//  setModified (false);
  return result;
}

unsigned int GPage::findIndexOfObject (GObject *obj)
{
  assert (obj->getLayer () != 0L);
  return obj->getLayer ()->findIndexOfObject (obj);
}

void GPage::insertObjectAtIndex (GObject* obj, unsigned int idx)
{
  obj->ref ();
  GLayer* layer = obj->getLayer ();
  if (layer == 0L)
    layer = active_layer;
  layer->insertObjectAtIndex (obj, idx);
  connect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
  connect (obj, SIGNAL(changed(const Rect&)),
           this, SLOT(objectChanged (const Rect&)));
//  setModified ();
  if (autoUpdate)
  {
    emit changed ();
    emit selectionChanged ();
  }
}

void GPage::moveObjectToIndex (GObject* obj, unsigned int idx)
{
  GLayer* layer = obj->getLayer ();
  if (layer == 0L)
    layer = active_layer;
  layer->moveObjectToIndex (obj, idx);

//  setModified ();
  if (autoUpdate)
  {
    emit changed ();
    emit selectionChanged ();
  }
}

KoPageLayout GPage::pageLayout ()
{
  return pLayout;
}

void GPage::setPageLayout (const KoPageLayout& layout)
{
  pLayout = layout;
  switch (layout.unit) {
  case PG_MM:
    paperWidth = (int) cvtMmToPt (pLayout.mmWidth);
    paperHeight = (int) cvtMmToPt (pLayout.mmHeight);
    break;
  case PG_PT:
    paperWidth = static_cast<int>(pLayout.ptWidth);
    paperHeight = static_cast<int>(pLayout.ptHeight);
    break;
  case PG_INCH:
    paperWidth = (int) cvtInchToPt (pLayout.inchWidth);
    paperHeight = (int) cvtInchToPt (pLayout.inchHeight);
    break;
  }
//  modifyFlag = true;
  emit sizeChanged ();
}

/*
 * Get an array with all layers of the document
 */
const QList<GLayer>& GPage::getLayers ()
{
  return layers;
}

/*
 * Set the active layer where further actions take place
 */
void GPage::setActiveLayer (GLayer *layer)
{
  QListIterator<GLayer> i(layers);
  for (; i.current(); ++i)
  {
    if ((*i) == layer)
    {
      active_layer = layer;
      unselectAllObjects();
      break;
    }
  }
}

/*
 * Retrieve the active layer
 */
GLayer* GPage::activeLayer ()
{
  return active_layer;
}

/*
 * Raise the given layer
 */
void GPage::raiseLayer (GLayer *layer)
{
  if (!layer || layer->isInternal ())
    return;

  if (layer == layers.last())
    // layer is already on top
    return;

  int pos=layers.findRef(layer);
  if(pos!=-1)
  {
    GLayer *l=layers.take(pos);
    layers.insert(pos+1, l);
    unselectAllObjects();
  }
  emit changed ();
}

/*
 * Lower the given layer
 */
void GPage::lowerLayer (GLayer *layer)
{
  if (!layer || layer->isInternal ())
    return;

  if (layer == layers.first())
    // layer is already at bottom
    return;

  int pos=layers.findRef(layer);
  if(pos!=-1)
  {
    GLayer *l=layers.take(pos);
    layers.insert(pos-1, l);
    unselectAllObjects();
  }
  emit changed ();
}

/*
 * Add a new layer on top of existing layers
 */
GLayer* GPage::addLayer ()
{
  GLayer* layer = new GLayer (this);
  connect (layer, SIGNAL(propertyChanged ()), this, SLOT(layerChanged ()));
  layers.append(layer);
  unselectAllObjects();
  return layer;
}

/*
 * Delete the given layer as well as all contained objects
 */
void GPage::deleteLayer (GLayer *layer)
{
  if (layer->isInternal ())
    return;

  if (layers.count() == 1)
    // we need at least one layer
    return;

  bool update = (active_layer == layer);

  int pos=layers.findRef(layer);
  if(pos!=-1) {
      // remove the layer from the array
      GLayer *l=layers.take(pos);
      // and delete the layer
      disconnect (l, SIGNAL(propertyChanged ()),
                  this, SLOT(layerChanged ()));
      delete l;

      if (update) {
          active_layer = layers.current();
          if(!active_layer)   // This one is needed for Qt 3.0 :)
              active_layer=layers.last();
          unselectAllObjects();
      }
  }
  emit selectionChanged ();
  emit changed ();
}

GLayer *GPage::layerForHelplines ()
{
  return layers.first();
}

bool GPage::helplineLayerIsActive ()
{
  return (active_layer->isInternal ()); // hmmm?? Is that safe?
}

void GPage::invalidateClipRegions ()
{
  for (QListIterator<GLayer> li(layers); li.current(); ++li)
  {
    if ((*li)->isVisible ())
    {
      const QList<GObject>& contents = (*li)->objects ();
      QListIterator<GObject> oi(contents);
      for (; oi.current(); ++oi)
        (*oi)->invalidateClipRegion ();
    }
  }
}

void GPage::selectNextObject ()
{
  GObject *newSel = 0L;

  if (selectionIsEmpty ())
  {
    newSel = const_cast< QList<GObject>& >(active_layer->objects()).first();
  }
  else
  {
    GObject *oldSel = selection.first();
    unsigned int idx = findIndexOfObject (oldSel);
    if (++idx >= active_layer->objects ().count())
      idx = 0;
    newSel = active_layer->objectAtIndex (idx);
  }
  setAutoUpdate (false);
  unselectAllObjects ();

  setAutoUpdate (true);
  if (newSel)
  {
    handle ().show (true);
    selectObject (newSel);
  }
}

void GPage::selectPrevObject ()
{
}

#include <GPage.moc>
