/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
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

#include "GDocument.h"

#include <qdom.h>

#include <klocale.h>

#include "kontour_global.h"
#include "kontour_doc.h"
#include "GPage.h"

using namespace Kontour;

// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.2";

GDocument::GDocument(KontourDocument *aDoc)
{
  mDoc = aDoc;

  mXRes = 72.0;
  mYRes = 72.0;

  mZoomFactor = 1.0;

  mXGridZ = 20.0;
  mYGridZ = 20.0;
  mXGrid = 20.0;
  mYGrid = 20.0;
  mShowGrid = false;
  mSnapToGrid = false;
  mGridColor = lightGray;

  mShowHelplines = true;
  mSnapToHelplines = false;
  mCurPageNum = 1;

  pages.setAutoDelete(true);
  pages.clear();

  mActivePage = addPage();

  mUpdateCanvas = true;

  changeCanvas();
}

GDocument::~GDocument()
{
  pages.clear();
}

void GDocument::updateCanvas(bool b)
{
  mUpdateCanvas = b;
}

void GDocument::zoomFactor(double factor)
{
  if(factor == mZoomFactor)
    return;

  if(factor > Kontour::maxZoomFactor)
    factor = Kontour::maxZoomFactor;
  if(factor < Kontour::minZoomFactor)
    factor = Kontour::minZoomFactor;

  /* Change grid distance. */
  mXGridZ = mXGrid * factor;
  while(mXGridZ < Kontour::minGridDistance)
    mXGridZ *= 2.0;
  mYGridZ = mYGrid * factor;
  while(mYGridZ < Kontour::minGridDistance)
    mYGridZ *= 2.0;

  double scale = factor / mZoomFactor;
  mZoomFactor = factor;
  changeCanvas();
  emit zoomFactorChanged(scale);
}

void GDocument::showGrid(bool flag)
{
  if(mShowGrid != flag)
  {
    mShowGrid = flag;
    emit gridChanged();   //TODO emit?
  }
  setModified();
}

void GDocument::snapToGrid(bool flag)
{
  mSnapToGrid = flag;
  setModified();
}

void GDocument::gridColor(QColor color)
{
  mGridColor = color;   //TODO emit?
  setModified();
}

void GDocument::setGridDistance(double hdist, double vdist)
{
  // TODO ZOOM!!!
  mXGridZ = hdist;
  mYGridZ = vdist;
}

void GDocument::showHelplines(bool flag)
{
  mShowHelplines = flag;
  setModified();
}

void GDocument::snapToHelplines(bool flag)
{
  mSnapToHelplines = flag;
  setModified();
}

void GDocument::horizHelplines(const QValueList<double> &lines)
{
  mHorizHelplines = lines;
}

void GDocument::vertHelplines(const QValueList<double> &lines)
{
  mVertHelplines = lines;
}

int GDocument::indexOfHorizHelpline(double pos)
{
  int ret = 0;
  for(QValueList<double>::Iterator i = mHorizHelplines.begin(); i != mHorizHelplines.end(); ++i, ++ret)
    if(pos - Kontour::nearDistance < *i && pos + Kontour::nearDistance > *i)
      return ret;
  return -1;
}

int GDocument::indexOfVertHelpline(double pos)
{
  int ret = 0;
  for(QValueList<double>::Iterator i = mVertHelplines.begin(); i != mVertHelplines.end(); ++i, ++ret)
    if(pos - Kontour::nearDistance < *i && pos + Kontour::nearDistance > *i)
      return ret;
  return -1;
}

void GDocument::updateHorizHelpline(int idx, double pos)
{
  mHorizHelplines[idx] = pos;
}

void GDocument::updateVertHelpline(int idx, double pos)
{
  mVertHelplines[idx] = pos;
}

void GDocument::addHorizHelpline(double pos)
{
  mHorizHelplines.append(pos);
}

void GDocument::addVertHelpline(double pos)
{
  mVertHelplines.append(pos);
}

QDomDocument GDocument::saveToXml()
{
  QDomDocument document = mDoc->createDomDocument("kontour", CURRENT_DTD_VERSION);
  QDomElement kontour = document.documentElement();
  kontour.setAttribute("editor", "Kontour 2.0");
  kontour.setAttribute("mime", "application/x-kontour");
  kontour.setAttribute("version", "1");

  QDomElement head = document.createElement("head");
  head.setAttribute("cpn", mCurPageNum);
  kontour.appendChild(head);

  QDomElement grid = document.createElement("grid");
  grid.setAttribute("dx", mXGrid);
  grid.setAttribute("dy", mYGrid);
  grid.setAttribute("align", mSnapToGrid ? 1 : 0);
  grid.setAttribute("show", mShowGrid ? 1 : 0);
  grid.setAttribute("color", mGridColor.name());
  head.appendChild(grid);

  QDomElement helplines = document.createElement("helplines");
  helplines.setAttribute("align", mSnapToHelplines ? 1 : 0);
  helplines.setAttribute("show", mShowHelplines ? 1 : 0);
  QValueList<double>::Iterator hi;
  for(hi = mHorizHelplines.begin(); hi != mHorizHelplines.end(); ++hi)
  {
    QDomElement hl = document.createElement("hl");
    hl.setAttribute("pos", (*hi));
    helplines.appendChild(hl);
  }
  for(hi = mVertHelplines.begin(); hi != mVertHelplines.end(); ++hi)
  {
    QDomElement vl = document.createElement("vl");
    vl.setAttribute("pos", (*hi));
    helplines.appendChild(vl);
  }
  head.appendChild(helplines);

  head.appendChild(mStyles.writeToXml(document));

  for(QPtrListIterator<GPage> pi(pages); pi.current(); ++pi)
  {
    GPage *p = (*pi);
    QDomElement page;
    page = p->saveToXml(document);
    kontour.appendChild(page);
  }
  setModified(false);
  return document;
}

bool GDocument::readFromXml(const QDomDocument &document)
{
  if(document.doctype().name() != "kontour")
    return false;
  QDomElement kontour = document.documentElement();
  if(kontour.attribute("mime") != "application/x-kontour" )
    return false;
  if(kontour.attribute("version") == "1")
  {
    QDomElement head = kontour.namedItem("head").toElement();
    mCurPageNum = head.attribute("cpn").toInt();

    QDomElement grid = head.namedItem("grid").toElement();
    mXGrid = grid.attribute("dx").toDouble();
    mYGrid = grid.attribute("dy").toDouble();
    mSnapToGrid = (grid.attribute("align").toInt() == 1);
    mShowGrid = (grid.attribute("show").toInt() == 1);
    mGridColor.setNamedColor(grid.attribute("color"));

    QDomElement helplines = grid.namedItem("helplines").toElement();
    mSnapToHelplines = (helplines.attribute("align").toInt() == 1);
    mShowHelplines = (helplines.attribute("show").toInt() == 1);

    QDomElement l = helplines.firstChild().toElement();
    for(; !l.isNull(); l = helplines.nextSibling().toElement())
      if(l.tagName() == "hl")
        mHorizHelplines.append(l.attribute("pos").toDouble());
      else if(l.tagName() == "vl")
        mVertHelplines.append(l.attribute("pos").toDouble());

    pages.clear();
    mActivePage = 0L;
    QDomNode n = kontour.firstChild();
    while(!n.isNull())
    {
      QDomElement pe = n.toElement();
      if(pe.tagName() == "page")
      {
        GPage *page = addPage();
        if(!mActivePage)
            mActivePage = page;
        page->readFromXml(pe);
      }
      n = n.nextSibling();
    }

    setModified(false);
    return true;
  }
  return false;
}

void GDocument::activePage(GPage *page)
{
  // TODO rewrite?
  QPtrListIterator<GPage> i(pages);
  for(; i.current(); ++i)
  {
    if((*i) == page)
    {
      mActivePage = page;
      emit pageChanged();
      break;
    }
  }
}

void GDocument::activePage(int i)
{
  mActivePage = pages.at(i);
  emit pageChanged();
}

GPage *GDocument::addPage()
{
  GPage *page = new GPage(this);
  pages.append(page);
  page->name(i18n("Page %1").arg(mCurPageNum));
  mCurPageNum++;
  emit updateLayerView();
  return page;
}

void GDocument::deletePage(GPage *page)
{
  // TODO test and rewrite (active page....)
  if(pages.count() == 1)
    return;

  int pos = pages.findRef(page);
  if(pos != -1)
  {
    /* remove the page from the array */
    GPage *p = pages.take(pos);
    /* and delete the page */
    delete p;
    emit updateLayerView();
  }
}

GPage *GDocument::pageForIndex(int i)
{
  return pages.at(i);
}

void GDocument::movePage(int from, int to, bool before)
{
  // TODO test and rewrite (active page....)
  if(!before)
    ++to;

  if(to > static_cast<int>(pages.count()))
  {
  }
  else
  {
    GPage *p = pages.take(from);
    if(from < to)
      pages.insert(to-1, p);
    else
      pages.insert(to, p);
  }
}

GPage *GDocument::findPage(QString name)
{
  for(QPtrListIterator<GPage> it(pages); it.current(); ++it)
    if(((GPage *)it)->name() == name)
      return (GPage *)it;
  return 0L;
}

void GDocument::emitChanged(const KoRect &r, bool handle)
{
  if(mUpdateCanvas)
    emit changed(r, handle);
}

void GDocument::emitSelectionChanged()
{
  emit selectionChanged();
}

void GDocument::setModified(bool flag)
{
  mDoc->setModified(flag);
}

void GDocument::changeCanvas()
{
  mXCanvas = static_cast<int>(activePage()->paperWidth() * mZoomFactor * mXRes / 72.0);
  mYCanvas = static_cast<int>(activePage()->paperHeight() * mZoomFactor * mYRes / 72.0);
}

#include "GDocument.moc"
