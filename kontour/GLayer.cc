/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#include "GLayer.h"

#include <qdom.h>

#include <klocale.h>
#include <kdebug.h>

#include "GPage.h"
#include "GObject.h"

const int LAYER_VISIBLE = 1;
const int LAYER_EDITABLE = 2;
const int LAYER_PRINTABLE = 4;

GLayer::GLayer(GPage *aPage)
{
  mPage = aPage;
  visibleFlag = true;
  printableFlag = true;
  editableFlag = true;
  wasEditable = true;
//    mName = i18n("Layer #") + QString::number(lastID++);
}

GLayer::~GLayer()
{
  for(GObject *o = contents.first(); o != 0L; o = contents.next())
  {
    if(o->isSelected())
      mPage->unselectObject(o);
    o->layer(0L);
//    o->unref (); //TODO
  }
  contents.clear ();
}

void GLayer::name(const QString &aName)
{
  mName = aName;
}

void GLayer::setVisible(bool flag)
{
  if(visibleFlag != flag)
  {
    visibleFlag = flag;
    if(!visibleFlag)
      editableFlag = false;
    else if(wasEditable)
      editableFlag = true;
    emit propertyChanged();
  }
}

void GLayer::setPrintable(bool flag)
{
  if(printableFlag != flag)
  {
    printableFlag = flag;
    emit propertyChanged();
  }
}

void GLayer::setEditable(bool flag)
{
  if(editableFlag != flag)
  {
    editableFlag = flag;
    if(editableFlag)
      visibleFlag = true;
    wasEditable = editableFlag;
    emit propertyChanged();
  }
}


QDomElement GLayer::saveToXml(QDomDocument &document)
{
  QDomElement layer = document.createElement("layer");
  int flags = (isVisible() ? LAYER_VISIBLE : 0) + (isPrintable() ? LAYER_PRINTABLE : 0) + (isEditable() ? LAYER_EDITABLE : 0);
  layer.setAttribute("id", name());
  layer.setAttribute("flags", QString::number(flags));
  for(QPtrListIterator<GObject> oi(contents); oi.current(); ++oi)
    layer.appendChild((*oi)->writeToXml(document));
  return layer;
}

bool GLayer::readFromXml(const QDomElement &layer)
{
/*  GObject *obj = 0L;
//  QDict<GObject> refDict;
  QString id = layer.attribute("id");
  if(id.isEmpty())
    return;
  setName(id);

  int flags = layer.attribute("flags").toInt();
  setVisible(flags & LAYER_VISIBLE);
  setPrintable(flags & LAYER_EDITABLE);
  setEditable(flags & LAYER_PRINTABLE);

  QDomNode cn = layer.firstChild();
  while(!cn.isNull())
  {
    QDomElement child = cn.toElement();
//    obj = KIllustrator::objectFactory(child, document()->document());
    if(!obj)
      kdDebug(38000) << "invalid object type: " << child.tagName() << endl;
    if (child.tagName() == "group")
      ((GGroup*)obj)->layer(this);
    if(obj->hasId())
          refDict.insert(obj->getId(), obj);
        insertObject(obj);
        cn=cn.nextSibling();
        newObjs.append(obj);
      }
    }
    n=n.nextSibling();
  }
//TODO change GOBject \/
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
  setAutoUpdate (true);*/
  return true;
}

void GLayer::insertObject(GObject *obj)
{
  obj->layer(this);
  contents.append(obj);
}

void GLayer::deleteObject(GObject *obj)
{
  GObject *o = contents.at(contents.findRef(obj));
  if(o != 0L)
  {
    o->layer(0L);
//    o->unref (); //TODO
    contents.removeRef(o);
  }
}

GObject *GLayer::findContainingObject(double x, double y)
{
  /*
    We are looking for the most relevant object, that means the object
    in front of all others. So, we have to start at the end of the list ...
  */
  GObject *o = contents.last();
  for(; o != 0L; o = contents.prev())
    if(o->contains(KoPoint(x, y)))
      return o;
  /* nothing found */
  return 0L;
}

int GLayer::findIndexOfObject(GObject *obj)
{
  return contents.findRef(obj);
}

void GLayer::insertObjectAtIndex(GObject* obj, unsigned int idx)
{
  contents.insert (idx, obj);
  obj->layer (this);
}

GObject *GLayer::objectAtIndex(unsigned int idx)
{
  return contents.at(idx);
}

void GLayer::moveObjectToIndex(GObject* obj, unsigned int idx)
{
  if(contents.removeRef(obj))
    contents.insert(idx, obj);
}

#include "GLayer.moc"
