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

#include "GPage.h"

#include <float.h>

#include <qdom.h>

#include <klocale.h>
#include <kdebug.h>

#include "kontour_global.h"
#include "GDocument.h"
#include "GObject.h"
#include "GLayer.h"
#include "units.h"

GPage::GPage(GDocument *aGDoc):
mHandle(this)
{
  mGDoc = aGDoc;
  mPageLayout = KoPageLayoutDia::standardLayout();
  // No margins
  mPageLayout.ptLeft = 0;
  mPageLayout.ptRight = 0;
  mPageLayout.ptTop = 0;
  mPageLayout.ptBottom = 0;
  // in pt !!
  mPaperWidth = (int)mPageLayout.ptWidth;
  mPaperHeight = (int)mPageLayout.ptHeight;

  mBGColor = white;

  mConvertibleCount = 0;
  mCurLayerNum = 1;

  selection.clear();
  layers.setAutoDelete(true);
  layers.clear();

  mActiveLayer = addLayer();
  mActiveLayer->setVisible(true);
  mActiveLayer->setPrintable(true);
  mActiveLayer->setEditable(true);
}

GPage::~GPage()
{
  layers.clear();
  selection.clear();
}

void GPage::name(QString aName)
{
  mName = aName;
}

void GPage::bgColor(QColor color)
{
  mBGColor = color;
}

void GPage::setPaperSize(int width, int height)
{
  mPaperWidth = width;
  mPaperHeight = height;
}

KoPageLayout GPage::pageLayout()
{
  return mPageLayout;
}

void GPage::pageLayout(const KoPageLayout &layout)
{
  mPageLayout = layout;
  mPaperWidth = static_cast<int>(mPageLayout.ptWidth);
  mPaperHeight = static_cast<int>(mPageLayout.ptHeight);
//  setModified();
//  emit sizeChanged ();
}

QDomElement GPage::saveToXml(QDomDocument &doc)
{
  // Many formats missing. Switching to KoPageFormat::formatString/formatFromString
  // would be the best thing to do, but this would require some conversion code... (DF)
/*  static const char* formats[] = {
    "a3", "a4", "a5", "us_letter", "us_legal", "screen", "custom"};
  static const char* orientations[] = {"portrait", "landscape"};
*/
  QDomElement page = doc.createElement("page");

  page.setAttribute("id", name());
//  page.setAttribute("bgcolor", mBGColor.name());  //TODO

/*  QDomElement layout = document.createElement("layout");
  layout.setAttribute ("format", formats[mPageLayout.format]);
  layout.setAttribute ("orientation", orientations[mPageLayout.orientation]);
  layout.setAttribute ("width", mPageLayout.mmWidth);
  layout.setAttribute ("height", mPageLayout.mmHeight);
  layout.setAttribute ("lmargin", mPageLayout.mmLeft);
  layout.setAttribute ("tmargin", mPageLayout.mmTop);
  layout.setAttribute ("rmargin", mPageLayout.mmRight);
  layout.setAttribute ("bmargin", mPageLayout.mmBottom);
  page.appendChild(layout);*/

  for(QPtrListIterator<GLayer> li(layers); li.current(); ++li)
  {
    GLayer *l=(*li);
    page.appendChild(l->saveToXml(doc));
  }
  return page;
}

bool GPage::readFromXml(const QDomElement &page)
{
  kdDebug(38000) << "Read page..." << endl;
  mName = page.attribute("id");
  QDomNode n = page.firstChild();
  layers.clear();
  mActiveLayer = 0L;
  while(!n.isNull())
  {
    QDomElement le = n.toElement();
    if(le.tagName() == "layer")
    {
      GLayer *layer = addLayer();
      if(!mActiveLayer)
        mActiveLayer = layer;
      layer->readFromXml(le);
    }
    n = n.nextSibling();
  }
  return true;
}

void GPage::activeLayer(GLayer *aLayer)
{
  QPtrListIterator<GLayer> i(layers);
  for (; i.current(); ++i)
  {
    if ((*i) == aLayer)
    {
      mActiveLayer = aLayer;
      unselectAllObjects();
      break;
    }
  }
}

GLayer* GPage::activeLayer()
{
  return mActiveLayer;
}

GLayer *GPage::addLayer()
{
  GLayer *layer = new GLayer (this);
  layer->name(i18n("Layer #%1").arg(mCurLayerNum));
  mCurLayerNum++;
  layers.append(layer);
  unselectAllObjects();
  return layer;
}

void GPage::deleteLayer(GLayer *layer)
{
  if (layers.count() == 1)
    // we need at least one layer
    return;

  bool update = (mActiveLayer == layer);

  int pos=layers.findRef(layer);
  if(pos!=-1) {
      // remove the layer from the array
      GLayer *l=layers.take(pos);
      // and delete the layer
      disconnect (l, SIGNAL(propertyChanged ()),
                  this, SLOT(layerChanged ()));
      delete l;

      if (update) {
          mActiveLayer = layers.current();
          if(!mActiveLayer)   // This one is needed for Qt 3.0 :)
              mActiveLayer=layers.last();
          unselectAllObjects();
      }
  }
//  emit selectionChanged();
//  emit changed();
}

void GPage::raiseLayer(GLayer *layer)
{
  if (!layer)
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
//  emit changed ();
}

void GPage::lowerLayer(GLayer *layer)
{
  if (!layer)
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
}

unsigned int GPage::objectCount() const
{
  unsigned int num = 0;
  for(QPtrListIterator<GLayer> i(layers); i.current(); ++i)
    num += (*i)->objectCount();
  return num;
}

void GPage::insertObject(GObject *obj)
{
  obj->ref();
  mActiveLayer->insertObject(obj);
}

void GPage::deleteObject(GObject *obj)
{
  GLayer *layer = obj->layer();
  if(layer->isEditable())
  {
    bool selected = obj->isSelected();
    if(selected)
    {
      selection.removeRef(obj);
      if(obj->isConvertible())
        mConvertibleCount--;
    }
    layer->deleteObject(obj);
    obj->unref();
    if(selected)
      updateSelection();
    else
      mGDoc->emitChanged(obj->boundingBox());
  }
}

unsigned int GPage::findIndexOfObject(GObject *obj)
{
  return obj->layer()->findIndexOfObject(obj);
}

void GPage::insertObjectAtIndex(GObject *obj, unsigned int idx)
{
  obj->ref();
  GLayer *layer = obj->layer();
  if(layer == 0L)
    layer = mActiveLayer;
  layer->insertObjectAtIndex (obj, idx);
}

void GPage::moveObjectToIndex(GObject *obj, unsigned int idx)
{
  GLayer *layer = obj->layer();
  if (layer == 0L)
    layer = mActiveLayer;
  layer->moveObjectToIndex (obj, idx);
}

void GPage::selectObject(GObject *obj)
{
  kdDebug(38000) << "Select object" << endl;
  if(selection.containsRef(obj) == 0)
  {
    /* object isn't yet in selection list */
    obj->select(true);
    if(obj->isConvertible())
      mConvertibleCount++;
    obj->ref();
    selection.append(obj);
    document()->emitSelectionChanged();
  }
}

void GPage::unselectObject(GObject *obj)
{
  int i = selection.findRef(obj);
  if(i != -1)
  {
    /* remove object from the selection list */
    obj->select(false);
    if(obj->isConvertible())
      mConvertibleCount--;
    obj->unref();
    selection.remove(i);
    updateSelection();
    document()->emitSelectionChanged();
  }
}

void GPage::selectAllObjects()
{
  selection.clear();
  for(QPtrListIterator<GLayer> i(layers); i.current(); ++i)
  {
    if((*i)->isEditable ())
    {
      const QPtrList<GObject> &contents = (*i)->objects();
      QPtrListIterator<GObject> oi(contents);
      for(; oi.current(); ++oi)
      {
        GObject *obj = *oi;
        obj->select(true);
        selection.append(obj);
      }
    }
  }
  calcBoxes();
  document()->emitSelectionChanged();
}

void GPage::unselectAllObjects()
{
  kdDebug(38000) << "Unselect all objects" << endl;
  if(selection.isEmpty())
    return;

  for(GObject *o = selection.first(); o != 0L; o = selection.next())
    o->select(false);
  selection.clear();
  calcBoxes();
  document()->emitSelectionChanged();
}

void GPage::selectNextObject()
{
  GObject *newSel = 0L;
//TODO if not editable layer?
  if(selectionIsEmpty())
    newSel = const_cast<QPtrList<GObject>&>(mActiveLayer->objects()).first();
  else
  {
    GObject *oldSel = selection.first();
    unsigned int idx = findIndexOfObject(oldSel);
    idx++;
    if(idx >= mActiveLayer->objects().count())
      idx = 0;
    kdDebug(38000) << "INDEX=" << idx << endl;
    newSel = mActiveLayer->objectAtIndex(idx);
  }
  unselectAllObjects();
  if(newSel)
    selectObject(newSel);
}

void GPage::deleteSelectedObjects()
{
  if (! selectionIsEmpty ())
  {
    for(QPtrListIterator<GObject> i(selection); i.current(); ++i)
    {
      GObject* obj = *i;
      disconnect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
      disconnect (obj, SIGNAL(changed(const KoRect&)),
                  this, SLOT(objectChanged (const KoRect&)));
      obj->layer ()->deleteObject (obj);
    }
    selection.clear ();
  }
}

void GPage::calcBoxes()
{
  //TODO private?
  if(selectionIsEmpty())
  {
    mBBox = KoRect();
    mSBox = KoRect();
    mHandle.empty();
  }
  else
  {
    QPtrListIterator<GObject> i(selection);
    mBBox = (*i)->boundingBox();
    mSBox = (*i)->shapeBox();
    ++i;
    for(; i.current(); ++i)
    {
      mBBox = mBBox.unite((*i)->boundingBox());
      mSBox = mSBox.unite((*i)->shapeBox());
    }
    mHandle.box(mBBox);
    mHandle.empty(false);
  }
}

void GPage::drawContents(KoPainter *p, const QWMatrix &m, bool withBasePoints, bool outline, bool withEditMarks)
{
  //TODO add regions
  for(QPtrListIterator<GLayer> i(layers); i.current(); ++i)
  {
    if((*i)->isVisible())
    {
      const QPtrList<GObject> &contents = (*i)->objects();
      QPtrListIterator<GObject> oi(contents);
      for(; oi.current(); ++oi)
        (*oi)->draw(p, m, withBasePoints && (*oi)->isSelected(), outline, withEditMarks);
    }
  }
}

GObject *GPage::findContainingObject(double x, double y)
{
  GObject *result = 0L;
  /*
     We are looking for the most relevant object, that means the object
     in front of all others. So, we have to start at the upper layer
  */
  QPtrListIterator<GLayer> i(layers);
  for(i.toLast(); i.current(); --i)
  {
    if((*i)->isEditable())
    {
      result = (*i)->findContainingObject(x, y);
      if(result)
        break;
    }
  }
  return result;
}

bool GPage::findContainingObjects(int x, int y, QPtrList<GObject> &olist)
{
  KoPoint coord(x, y);
  for (QPtrListIterator<GLayer> li(layers); li.current(); ++li)
  {
    if ((*li)->isEditable ())
    {
      const QPtrList<GObject>& contents = (*li)->objects ();
      QPtrListIterator<GObject> oi(contents);
      for ( ; oi.current(); ++oi)
        if ((*oi)->contains (coord))
          olist.append (*oi);
    }
  }
  return olist.count () > 0;
}

bool GPage::findObjectsContainedIn(const KoRect &r, QPtrList<GObject> &olist)
{
  for(QPtrListIterator<GLayer> li(layers); li.current(); ++li)
  {
    if ((*li)->isEditable ())
    {
      const QPtrList<GObject> &contents = (*li)->objects();
      QPtrListIterator<GObject> oi(contents);
      for ( ; oi.current(); ++oi)
        if (r.contains ((*oi)->boundingBox ()))
          olist.append (*oi);
    }
  }
  return olist.count() > 0;
}

void GPage::updateSelection()
{
  // TODO may be not recalculate box?
  KoRect r = mBBox;
  calcBoxes();
  r = r.unite(mBBox);
  document()->emitChanged(r, true);
}

/*******************[OLD]*********************
bool GPage::readFromXml (const QDomElement &page)
{
  setAutoUpdate (false);

  setName(page.attribute("id"));
  mBGColor.setNamedColor(page.attribute("bgcolor", "#FFFFFF"));
  QDomElement layout=page.namedItem("layout").toElement();
  QString tmp=layout.attribute("format");
  if (tmp == "a3")
    mPageLayout.format = PG_DIN_A3;
  else
    if (tmp == "a4")
      mPageLayout.format = PG_DIN_A4;
    else
      if (tmp == "a5")
        mPageLayout.format = PG_DIN_A5;
      else
      if (tmp == "usletter")
        mPageLayout.format = PG_US_LETTER;
      else
        if (tmp == "uslegal")
          mPageLayout.format = PG_US_LEGAL;
        else
	  if (tmp == "custom")
            mPageLayout.format = PG_CUSTOM;
          else
            mPageLayout.format = PG_DIN_A4;

  tmp=layout.attribute("orientation");
  if (tmp == "portrait")
    mPageLayout.orientation = PG_PORTRAIT;
  else
    if (tmp == "landscape")
      mPageLayout.orientation = PG_LANDSCAPE;
    else
      mPageLayout.orientation = PG_PORTRAIT;

  mPageLayout.mmWidth=layout.attribute("width").toFloat();
  mPageLayout.mmHeight=layout.attribute("height").toFloat();
  mPageLayout.mmLeft=layout.attribute("lmargin").toFloat();
  mPageLayout.mmRight=layout.attribute("rmargin").toFloat();
  mPageLayout.mmBottom=layout.attribute("bmargin").toFloat();
  mPageLayout.mmTop=layout.attribute("tmargin").toFloat();

// update page layout
  setPageLayout (mPageLayout);

  QPtrList<GObject> dummy;
  bool result = parseBody (page, dummy, false);

  setModified (false);
  return result;
}

bool GPage::insertFromXml (const QDomDocument &document, QPtrList<GObject>& newObjs)
{
  if ( document.doctype().name() != "killustrator" )
    return false;
  QDomElement doc = document.documentElement();

  if ( doc.attribute( "mime" ) != KILLUSTRATOR_MIMETYPE )
    return false;
  return parseBody (doc, newObjs, true);
}

bool GPage::parseBody (const QDomElement &element, QPtrList<GObject>& newObjs, bool markNew)
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
        mActiveLayer = addLayer ();
        mActiveLayer->setName (id);
        int flags = layerelem.attribute("flags").toInt();
        mActiveLayer->setVisible (flags & LAYER_VISIBLE);
        mActiveLayer->setPrintable (flags & LAYER_EDITABLE);
        mActiveLayer->setEditable (flags & LAYER_PRINTABLE);
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
//                      obj = proto->create (this, child);
//                   }
//                   else
                      kdDebug(38000) << "invalid object type: " << child.tagName() << endl;
//                }
        if (child.tagName() == "group")
          ((GGroup*)obj)->setLayer (mActiveLayer);
        if(obj->hasId())
          refDict.insert(obj->getId(), obj);
        insertObject(obj);
        cn=cn.nextSibling();
        newObjs.append(obj);
      }
    }
    n=n.nextSibling();
  }

// update object connections
  for (QPtrListIterator<GLayer> i(layers); i.current(); ++i)
  {
    const QPtrList<GObject>& contents = (*i)->objects ();
    for (QPtrListIterator<GObject> oi(contents); oi.current(); ++oi)
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
*/

#include "GPage.moc"
