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

#if defined(__FreeBSD__) || defined(__NetBSD__)
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
:doc(_doc)
{
  initialize ();
}

GDocument::~GDocument ()
{
  pages.clear();
}

void GDocument::initialize ()
{
  gridx = 20.0;
  gridy = 20.0;
  gridSnapIsOn = false;
  gridIsOn = false;
  mGridColor = lightGray;

  helplinesSnapIsOn = false;
  helplinesAreOn = true;

  modifyFlag = false;
  filename = i18n("<unnamed>");

  pages.setAutoDelete(true);
  pages.clear ();

  active_page = addPage();
  active_page->setName(i18n("Page %1").arg(1));
  curPageNum = 2;

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

void GDocument::emitChanged()
{
  emit changed();
}

void GDocument::emitHandleChanged()
{
  emit handleChanged();
}

void GDocument::emitChanged(const Rect& r)
{
  emit changed(r);
}

void GDocument::setActivePage (int i)
{
  active_page = pages.at(i);
  emit pageChanged();
}

GPage *GDocument::pageForIndex (int i)
{
  return pages.at(i);
}

GPage *GDocument::addPage ()
{
  GPage *aPage = new GPage (this);
  pages.append(aPage);
  aPage->setName(i18n("Page %1").arg(curPageNum));
  curPageNum++;
  return aPage;
}

void GDocument::movePage( int from, int to, bool before )
{

  if ( !before )
    ++to;

  if ( to > static_cast<int>(pages.count()) )
  {
   kdDebug(0) << "?" <<endl;
  }
  else
  {
    GPage *p=pages.take(from);
    if(from < to)
      pages.insert( to-1, p );
    else
      pages.insert( to, p );
  }
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
  for(QListIterator<GPage> it(pages); it.current(); ++it)
    if(((GPage *)it)->name() == name)
      return (GPage *)it;
  return 0;
}

void GDocument::setAutoUpdate (bool flag)
{
  autoUpdate = flag;
  if (autoUpdate)
  {
    selBoxIsValid = false;
    activePage()->updateHandle ();
    emit changed ();
  }
}

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
  head.setAttribute ("currentpagenum", curPageNum);
  killustrator.appendChild(head);

  QDomElement grid=document.createElement("grid");
  grid.setAttribute ("dx", gridx);
  grid.setAttribute ("dy", gridy);
  grid.setAttribute ("align", gridSnapIsOn ? 1 : 0);
  grid.setAttribute ("show", gridIsOn ? 1 : 0);
  grid.setAttribute ("color", mGridColor.name());
  head.appendChild(grid);

  QDomElement helplines=document.createElement("helplines");
  helplines.setAttribute ("align", helplinesSnapIsOn ? 1 : 0);
  helplines.setAttribute ("show", helplinesAreOn ? 1 : 0);
  QValueList<float>::Iterator hi;
  for(hi = hHelplines.begin(); hi != hHelplines.end(); ++hi)
  {
    QDomElement hl=document.createElement("hl");
    hl.setAttribute ("pos", (*hi));
    helplines.appendChild(hl);
  }
  for(hi = vHelplines.begin(); hi != vHelplines.end(); ++hi)
  {
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
    killustrator.appendChild(page);
  }
  setModified (false);
  return document;
}

bool GDocument::readFromXml (const  QDomDocument &document)
{
  kdDebug(38000)<<"GDocument::readFromXml()"<<endl;
  if ( document.doctype().name() != "killustrator" )
    return false;
  QDomElement killustrator = document.documentElement();
  if ( killustrator.attribute( "mime" ) != KILLUSTRATOR_MIMETYPE )
    return false;
  if( killustrator.attribute("version")=="3")
  {
    QDomElement head=killustrator.namedItem("head").toElement();
    setAutoUpdate (false);
    curPageNum = head.attribute("currentpagenum").toInt();

    QDomElement grid=head.namedItem("grid").toElement();
    gridx=grid.attribute("dx").toFloat();
    gridy=grid.attribute("dy").toFloat();
    gridSnapIsOn = (grid.attribute("align").toInt() == 1);
    gridIsOn = (grid.attribute("show").toInt() == 1);
    mGridColor.setNamedColor(grid.attribute("color"));
    
    QDomElement helplines=grid.namedItem("helplines").toElement();
    helplinesSnapIsOn = (helplines.attribute("align").toInt() == 1);
    helplinesAreOn = (helplines.attribute("show").toInt() == 1);

    QDomElement l=helplines.firstChild().toElement();
    for( ; !l.isNull(); l=helplines.nextSibling().toElement())
    {
      if(l.tagName()=="hl")
        hHelplines.append(l.attribute("pos").toFloat());
      else
        if(l.tagName()=="vl")
          vHelplines.append(l.attribute("pos").toFloat());
    }

    pages.clear ();
    QDomNode n = killustrator.firstChild();
    while(!n.isNull())
    {
      QDomElement pe=n.toElement();
      kdDebug(0) << "Tag=" << pe.tagName() << endl;
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
  if( killustrator.attribute("version") == "2")
  {
    QDomElement head = killustrator.namedItem("head").toElement();
    setAutoUpdate (false);

    QDomElement grid = head.namedItem("grid").toElement();
    gridx = grid.attribute("dx").toFloat();
    gridy = grid.attribute("dy").toFloat();
    gridSnapIsOn = (grid.attribute("align").toInt() == 1);

    QDomElement helplines=grid.namedItem("helplines").toElement();
    helplinesSnapIsOn = (helplines.attribute("align").toInt()==1);

    QDomElement l = helplines.firstChild().toElement();
    for( ; !l.isNull(); l=helplines.nextSibling().toElement())
    {
      if(l.tagName() == "hl")
        hHelplines.append(l.attribute("pos").toFloat());
      else
        if(l.tagName() == "vl")
          vHelplines.append(l.attribute("pos").toFloat());
    }

    pages.clear ();

    GPage *page = addPage();
    page->readFromXmlV2(killustrator);

    page->setName(i18n("Page %1").arg(1));
    curPageNum = 2;

    setModified (false);
    emit gridChanged ();
    return true;
  }

  kdError(38000) << "Sorry, KIllustrator's current file format is incompatible to the old format." << endl;
  return false;
}

// called from internal layer when visible flag was changed
void GDocument::helplineStatusChanged ()
{
  emit gridChanged ();
}

/****************[Helplines]*****************/

void GDocument::setHorizHelplines (const QValueList<float>& lines)
{
  hHelplines = lines;
  if (helplinesAreOn);
}

void GDocument::setVertHelplines (const QValueList<float>& lines)
{
  vHelplines = lines;
  if (helplinesAreOn);
}

void GDocument::alignToHelplines (bool flag)
{
  if (helplinesSnapIsOn != flag)
  {
    helplinesSnapIsOn = flag;
  }
}

void GDocument::showHelplines (bool flag)
{
  if (helplinesAreOn != flag)
  {
    helplinesAreOn = flag;
  }
}

int GDocument::indexOfHorizHelpline (float pos)
{
  int ret=0;
  for (QValueList<float>::Iterator i = hHelplines.begin(); i!=hHelplines.end(); ++i, ++ret)
    if (pos - NEAR_DISTANCE < *i && pos + NEAR_DISTANCE > *i)
      return ret;
  return -1;
}

int GDocument::indexOfVertHelpline (float pos)
{
  int ret=0;
  for (QValueList<float>::Iterator i = vHelplines.begin(); i!=vHelplines.end(); ++i, ++ret)
    if (pos - NEAR_DISTANCE < *i && pos + NEAR_DISTANCE > *i)
      return ret;
  return -1;
}

void GDocument::updateHorizHelpline (int idx, float pos)
{
  hHelplines[idx] = pos;
}

void GDocument::updateVertHelpline (int idx, float pos)
{
  vHelplines[idx] = pos;
}

void GDocument::addHorizHelpline(float pos)
{
  hHelplines.append(pos);
  if (helplinesAreOn);
}

void GDocument::addVertHelpline(float pos)
{
  vHelplines.append(pos);
  if (helplinesAreOn);
}

/****************[Grid]**********************/

void GDocument::showGrid (bool flag)
{
  if (gridIsOn != flag)
  {
    gridIsOn = flag;
  }
}

void GDocument::snapToGrid (bool flag)
{
  if(gridSnapIsOn != flag)
  {
    gridSnapIsOn = flag;
  }
}

void GDocument::gridColor(QColor color)
{
  mGridColor = color;
}

void GDocument::setGridDistance (float hdist, float vdist)
{
  gridx = hdist;
  gridy = vdist;
}

#include <GDocument.moc>
