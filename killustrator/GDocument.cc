/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include "GDocument.h"
#include "GPage.h"

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

#include <iostream.h>

#define LAYER_VISIBLE   1
#define LAYER_EDITABLE  2
#define LAYER_PRINTABLE 4

GDocument::GDocument (KIllustratorDocument *_doc)
:doc(_doc),selHandle(this)
{
  initialize ();
}

GDocument::~GDocument ()
{
  pages.clear();
}

void GDocument::initialize ()
{
  gridx = gridy = 20.0;
  snapToGrid = snapToHelplines = false;

  modifyFlag = false;
  filename = i18n("<unnamed>");

  pages.setAutoDelete(true);
  pages.clear ();

  active_page = addPage();
  
  autoUpdate = true;
  emit changed ();
  emit sizeChanged();
}

const QList<GPage>& GDocument::getPages ()
{
  return pages;
}

GPage *GDocument::activePage()
{
  return active_page;
}

void GDocument::setActivePage (GPage *page)
{
  QListIterator<GPage> i(pages);
  for (; i.current(); ++i)
  {
    if ((*i) == page)
    {
      active_page = page;
      break;
    }
  }
}

GPage *GDocument::addPage ()
{
  GPage *aPage = new GPage (this);
  pages.append(aPage);
  return aPage;
}

void GDocument::deletePage (GPage *pg)
{
  if (pages.count() == 1)
    return;

  int pos=pages.findRef(pg);
  if(pos!=-1)
  {
      // remove the page from the array
      GPage *p=pages.take(pos);

      // and delete the page
      delete p;
  }
}

GPage *GDocument::findPage(QString name)
{

}

void GDocument::setAutoUpdate (bool flag) {
  autoUpdate = flag;
  if (autoUpdate) {
    selBoxIsValid = false;
//    updateHandle ();
    emit changed ();
  }
}

void GDocument::emitHandleChanged()
{
   emit handleChanged();
};

void GDocument::setModified (bool flag)
{
  modifyFlag = flag;
  emit wasModified (flag);
}

QDomDocument GDocument::saveToXml ()
{

    QDomDocument document("killustrator");
    document.appendChild( document.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement killustrator=document.createElement("killustrator");
    killustrator.setAttribute("editor", "KIllustrator");
    killustrator.setAttribute ("mime", KILLUSTRATOR_MIMETYPE);
    killustrator.setAttribute("version", "3");
    document.appendChild(killustrator);

    QDomElement head=document.createElement("head");
    killustrator.appendChild(head);

    QDomElement grid=document.createElement("grid");
    grid.setAttribute ("dx", gridx);
    grid.setAttribute ("dy", gridy);
    grid.setAttribute ("align", snapToGrid ? 1 : 0);
    head.appendChild(grid);

    QDomElement helplines=document.createElement("helplines");
    helplines.setAttribute ("align", snapToHelplines ? 1 : 0);
    QValueList<float>::Iterator hi;
    for(hi = hHelplines.begin(); hi!=hHelplines.end(); ++hi) {
        QDomElement hl=document.createElement("hl");
        hl.setAttribute ("pos", (*hi));
        helplines.appendChild(hl);
    }
    for(hi = vHelplines.begin(); hi!=vHelplines.end(); ++hi) {
        QDomElement vl=document.createElement("vl");
        vl.setAttribute ("pos", (*hi));
        helplines.appendChild(vl);
    }
    grid.appendChild(helplines);

    for (QListIterator<GPage> pi(pages); pi.current(); ++pi)
    {
      GPage *p = (*pi);

      QDomElement page;
      page = p->saveToXml(document);
    }
    setModified (false);
    return document;
}

bool GDocument::insertFromXml (const QDomDocument& /*document*/, QList<GObject>& /*newObjs*/)
{
/*    if ( document.doctype().name() != "killustrator" )
        return false;
    QDomElement doc = document.documentElement();

    if ( doc.attribute( "mime" ) != KILLUSTRATOR_MIMETYPE )
        return false;*/
    return false;// parseBody (doc, newObjs, true);
}

bool GDocument::readFromXml (const  QDomDocument &document)
{
  kdDebug()<<"GDocument::readFromXml()"<<endl;
  if ( document.doctype().name() != "killustrator" )
    return false;
  QDomElement killustrator = document.documentElement();
  if ( killustrator.attribute( "mime" ) != KILLUSTRATOR_MIMETYPE )
    return false;
  if( killustrator.attribute("version")!="3")
  {
    kdError(38000) << "Sorry, KIllustrator's current file format is incompatible to the old format." << endl;
    return false;
  }

  QDomElement head=killustrator.namedItem("head").toElement();
  setAutoUpdate (false);

  QDomElement grid=head.namedItem("grid").toElement();
  gridx=grid.attribute("dx").toFloat();
  gridy=grid.attribute("dy").toFloat();
  snapToGrid=(grid.attribute("align").toInt()==1);

  QDomElement helplines=grid.namedItem("helplines").toElement();
  snapToHelplines=(helplines.attribute("align").toInt()==1);

  QDomElement l=helplines.firstChild().toElement();
  for( ; !l.isNull(); l=helplines.nextSibling().toElement())
  {
    if(l.tagName()=="hl")
      hHelplines.append(l.attribute("pos").toFloat());
    else
      if(l.tagName()=="vl")
        vHelplines.append(l.attribute("pos").toFloat());
  }

  QDomNode n = killustrator.firstChild();
  while(!n.isNull())
  {
    QDomElement pe=n.toElement();
    if (pe.tagName() == "page")
    {
      GPage *page = addPage();
      page->readFromXml(pe);
    }
    n=n.nextSibling();
  }

  setModified (false);
  emit gridChanged ();
  return true;
}

void GDocument::setGrid (float dx, float dy, bool snap)
{
  gridx = dx;
  gridy = dy;
  snapToGrid = snap;
}

void GDocument::getGrid (float& dx, float& dy, bool& snap)
{
  dx = gridx;
  dy = gridy;
  snap = snapToGrid;
}

void GDocument::setHelplines (const QValueList<float>& hlines,
                              const QValueList<float>& vlines,
                              bool snap)
{
  hHelplines = hlines;
  vHelplines = vlines;
  snapToHelplines = snap;
}

void GDocument::getHelplines (QValueList<float>& hlines, QValueList<float>& vlines,
                              bool& snap)
{
  hlines = hHelplines;
  vlines = vHelplines;
  snap = snapToHelplines;
}

// called from internal layer when visible flag was changed
void GDocument::helplineStatusChanged ()
{
  emit gridChanged ();
}

#include <GDocument.moc>
