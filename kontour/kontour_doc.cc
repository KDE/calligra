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

#include "kontour_doc.h"

#include <qpainter.h>
#include <qfileinfo.h>

#include <klocale.h>
#include <koTemplateChooseDia.h>

#include "kontour_factory.h"
#include "kontour_view.h"
#include "GDocument.h"
//#include "GPage.h"

/**********************[KontourChild]**********************/
KontourChild::KontourChild(KontourDocument *kontour, KoDocument *doc, const QRect &geometry)
:KoDocumentChild(kontour, doc, geometry)
{
}

KontourChild::~KontourChild()
{
}

KoDocument *KontourChild::hitTest(const QPoint &, const QWMatrix &)
{
  return 0L;
}

/**********************[KontourDocument]**********************/

KontourDocument::KontourDocument(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, bool singleViewMode)
:KoDocument(parentWidget, widgetName, parent, name, singleViewMode)
{
  setInstance(KontourFactory::global());
  mGDoc = new GDocument(this);
  mHistory = new KCommandHistory(actionCollection(), true);
}

KontourDocument::~KontourDocument()
{
  delete mHistory;
  delete mGDoc;
}

void KontourDocument::paintContent(QPainter &/*painter*/, const QRect &/*rect*/, bool /*transparent*/, double /*zoomX*/, double /*zoomY*/)
{
//  painter.save();
//  painter.scale(zoomX,zoomY);
//  KoRect r( (double)rect.x(), (double)rect.y(), (double)rect.width()/zoomX, (double)rect.height()/zoomY );

    /*else
    {
       Canvas *can=m_killuView->getCanvas();
       float newZoom=zoomX;
       if (newZoom!=can->getZoomFactor())
          can->setZoomFactor(newZoom);
    }*/

//  if(!transparent)
//    painter.fillRect(rect, white);
  //mGDocument->activePage()->drawContentsInRegion(painter, r, r);
//  painter.restore();
}

bool KontourDocument::initDoc()
{
  QString templ;
  KoTemplateChooseDia::ReturnType ret;

  ret = KoTemplateChooseDia::choose(KontourFactory::global(), templ,
    "application/x-kontour", "*.kon", i18n("Kontour"), KoTemplateChooseDia::Everything, "kontour_template");
  if(ret == KoTemplateChooseDia::Template)
  {
    QFileInfo fileInfo (templ);
    QString fileName (fileInfo.dirPath(true) + "/" + fileInfo.baseName() + ".kon");
    bool ok = loadNativeFormat(fileName);
    setModified(true);
    return ok;
  }
  else if(ret == KoTemplateChooseDia::File)
  {
    KURL url;
    url.setPath(templ);
    bool ok = openURL(url);
    return ok;
  }
  else if( ret == KoTemplateChooseDia::Empty )
    return true;
  else
    return false;
}

void KontourDocument::addShell(KoMainWindow */*shell*/)
{
}

bool KontourDocument::loadXML(QIODevice *, const QDomDocument &doc)
{
  if(mGDoc->readFromXml(doc))
  {
/*    kdDebug(38000) << "load embedded objects"<<endl;
    // now look for part objects in order to create the child list
    QPtrListIterator<GPage> p(m_gdocument->getPages());
    for(; p.current(); ++p)
    {
      QPtrListIterator<GLayer> i((*p)->getLayers());
      for(; i.current(); ++i)
      {
        GLayer *layer = *i;
        const QPtrList<GObject> &contents = layer->objects();
        for(QPtrListIterator<GObject> oi(contents); oi.current(); ++oi)
          if ((*oi)->isA ("GPart"))
  	  {
	    kdDebug(38000) << "inserting object"<<endl;
            GPart *part = (GPart *)*oi;
            insertChild(part->getChild());
          }
      }
      return true;
    }*/
    return true;
  }
  return false;
}

bool KontourDocument::loadChildren(KoStore *store)
{
  QPtrListIterator<KoDocumentChild> it(children());
  for(; it.current(); ++it)
    if(!((KoDocumentChild*)it.current())->loadDocument(store))
      return false;
  return true;
}

bool KontourDocument::completeLoading(KoStore *)
{
  return true;
}

QDomDocument KontourDocument::saveXML()
{
  return mGDoc->saveToXml();
}

bool KontourDocument::saveChildren(KoStore *store)
{
  int i = 0;
  QPtrListIterator<KoDocumentChild> it(children());
  for(; it.current(); ++it)
  {
    if(!((KoDocumentChild*)it.current())->document()->saveToStore(store, QString::number(i++)))
      return false;
  }
  return true;
}

bool KontourDocument::completeSaving(KoStore *)
{
  return true;
}

KoView *KontourDocument::createViewInstance(QWidget *parent, const char *name)
{
  return new KontourView(parent, name, this);
}

void KontourDocument::insertChild(KoDocumentChild *child)
{
  KoDocument::insertChild( child );
  setModified (true);
}

void KontourDocument::insertPart(const QRect &/*rect*/, KoDocumentEntry &/*e*/)
{
/*  KoDocument *doc = e.createDoc(this);

  if(!doc || !doc->initDoc())
  {
    KMessageBox::error(0, i18n("Could not insert document"));
    return;
  }

  KontourChild *child = new KontourChild(this, doc, rect);
  insertChild( child );

  GPart *part = new GPart(mGDocument, child);
  mGDocument->activePage()->insertObject(part);
  emit partInserted(child, part);*/
}

void KontourDocument::changeChildGeometry (KontourChild* /*child*/, const QRect& /*r*/)
{
/*  child->setGeometry (r);
  setModified (true);
  emit childGeometryChanged(child);*/
}

void KontourDocument::modified(bool m)
{
  setModified(m);
}

#include "kontour_doc.moc"
