/***************************************************************************
                          kudesignerdoc.cpp  -  description
                             -------------------
    begin                : Thu Jun  6 11:31:39 EEST 2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for Qt
#include <qdir.h>
#include <qwidget.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qdom.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <ktempfile.h>

// application specific includes
#include "kudesignerdoc.h"
#include "kudesigner.h"
#include "kudesignerview.h"
#include "mycanvas.h"
#include "canvbox.h"
#include "creportitem.h"
#include "clabel.h"
#include "cfield.h"
#include "cspecialfield.h"
#include "ccalcfield.h"
#include "cline.h"

QPtrList<KuDesignerView> *KuDesignerDoc::pViewList = 0L;

KuDesignerDoc::KuDesignerDoc(QWidget *parent, const char *name) : QObject(parent, name)
{
    docCanvas = 0;
    modified = false;

  if(!pViewList)
  {
    pViewList = new QPtrList<KuDesignerView>();
  }

  pViewList->setAutoDelete(true);
}

KuDesignerDoc::~KuDesignerDoc()
{
    delete pViewList;
}

void KuDesignerDoc::addView(KuDesignerView *view)
{
  pViewList->append(view);
}

void KuDesignerDoc::removeView(KuDesignerView *view)
{
  pViewList->remove(view);
}
void KuDesignerDoc::setURL(const KURL &url)
{
  doc_url=url;
}

const KURL& KuDesignerDoc::URL() const
{
  return doc_url;
}

void KuDesignerDoc::slotUpdateAllViews(KuDesignerView *sender)
{
  KuDesignerView *w;
  if(pViewList)
  {
    for(w=pViewList->first(); w!=0; w=pViewList->next())
    {
      if(w!=sender)
        w->repaint();
    }
  }

}

bool KuDesignerDoc::saveModified()
{
  bool completed=true;

  if(modified)
  {
    KuDesignerApp *win=(KuDesignerApp *) parent();
    int want_save = KMessageBox::warningYesNoCancel(win,
                                         i18n("The current file has been modified.\n"
                                              "Do you want to save it?"),
                                         i18n("Warning"));
    switch(want_save)
    {
      case KMessageBox::Yes:
           if (doc_url.fileName() == i18n("Untitled"))
           {
             win->slotFileSaveAs();
           }
           else
           {
             saveDocument(URL());
       	   };

       	   deleteContents();
           completed=true;
           break;

      case KMessageBox::No:
           setModified(false);
           deleteContents();
           completed=true;
           break;

      case KMessageBox::Cancel:
           completed=false;
           break;

      default:
           completed=false;
           break;
    }
  }

  return completed;
}

void KuDesignerDoc::closeDocument()
{
  deleteContents();
}

bool KuDesignerDoc::newDocument(QSize pSize, int top, int bottom, int left, int right,
    bool orientation,int pageSize)
{
  /////////////////////////////////////////////////
  // TODO: Add your document initialization code here
  /////////////////////////////////////////////////
  //getting the page width and height
  	int height = 297;
  	int width = 210;
  	height = pSize.height();
  	width = pSize.width();
  	if (orientation)
  	{
  	    int temp = height;
  	    height = width;
  	    width = temp;
  	}

  	//zooming canvas according to screen resolution
  	QPrinter* printer;

  	// Set the page size
  	printer = new QPrinter();
  	printer->setFullPage(true);
  	printer->setPageSize((QPrinter::PageSize)pageSize);
  	printer->setOrientation((QPrinter::Orientation)orientation);

  	// Get the page metrics and set appropriate wigth and height
  	QPaintDeviceMetrics pdm(printer);
  	width = pdm.width();
  	height = pdm.height();

  	//this is not needed anymore
  	delete printer;


   	//creating canvas
  	docCanvas = new MyCanvas(width, height);
  	docCanvas->setAdvancePeriod(30);

  	//creating KugarTemplate object
  	CanvasKugarTemplate *templ = new CanvasKugarTemplate(0, 0, width, height, docCanvas);
  	templ->show();
  	templ->props["PageSize"].first = QString("%1").arg(pageSize);
  	templ->props["PageOrientation"].first = QString("%1").arg(orientation);
  	templ->props["TopMargin"].first = QString("%1").arg(top);
  	templ->props["BottomMargin"].first = QString("%1").arg(bottom);
  	templ->props["LeftMargin"].first = QString("%1").arg(left);
  	templ->props["RightMargin"].first = QString("%1").arg(right);

    modified=true;
    doc_url.setFileName(i18n("Untitled"));

  return true;
}



void KuDesignerDoc::setReportItemAttributes(QDomNode *node, CanvasReportItem *item)
{
    QDomNamedNodeMap attributes = node->attributes();

    for (unsigned int i = 0; i < attributes.count(); i++)
    {
	item->props[attributes.item(i).nodeName()].first = attributes.item(i).nodeValue();
    }
}

void KuDesignerDoc::addReportItems(QDomNode *node, CanvasBand *section)
{
    QDomNodeList children = node->childNodes();
    int childCount = children.length();

    for (int j = 0; j < childCount; j++)
    {
	QDomNode child = children.item(j);
	if(child.nodeType() == QDomNode::ElementNode)
	{
	    if (child.nodeName() == "Line")
	    {
		CanvasLine *line = new CanvasLine(0, 0, 50, 20, docCanvas);
		section->items.append(line);
		setReportItemAttributes(&child, line);
		line->setSectionUndestructive(section);
//		line->show();
	    }
	    else if (child.nodeName() == "Label")
	    {
		CanvasLabel *label = new CanvasLabel(0, 0, 50, 20, docCanvas);
		section->items.append(label);
		setReportItemAttributes(&child, label);
		label->setSectionUndestructive(section);
//		label->show();
	    }
	    else if (child.nodeName() == "Special")
	    {
		CanvasSpecialField *special = new CanvasSpecialField(0, 0, 50, 20, docCanvas);
		section->items.append(special);
		setReportItemAttributes(&child, special);
		special->setSectionUndestructive(section);
//		special->show();
	    }
	    else if (child.nodeName() == "Field")
	    {
		CanvasField *field = new CanvasField(0, 0, 50, 20, docCanvas);
		section->items.append(field);
		setReportItemAttributes(&child, field);
		field->setSectionUndestructive(section);
//		field->show();
	    }
	    else if (child.nodeName() == "CalculatedField")
	    {
		CanvasCalculatedField *calcField = new CanvasCalculatedField(0, 0, 50, 20, docCanvas);
		section->items.append(calcField);
		setReportItemAttributes(&child, calcField);
		calcField->setSectionUndestructive(section);
//		calcField->show();
	    }
	}
    }
}

void KuDesignerDoc::setReportHeaderAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasReportHeader *rh = new CanvasReportHeader(docCanvas->templ->props["LeftMargin"].first.toInt(),
		0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"].first.toInt() -
		docCanvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    rh->props["Height"].first = attributes.namedItem("Height").nodeValue();
    docCanvas->templ->reportHeader = rh;
    addReportItems(node, rh);
}

void KuDesignerDoc::setReportFooterAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasReportFooter *rf = new CanvasReportFooter(docCanvas->templ->props["LeftMargin"].first.toInt(),
		0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"].first.toInt() -
		docCanvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    rf->props["Height"].first = attributes.namedItem("Height").nodeValue();
    docCanvas->templ->reportFooter = rf;
    addReportItems(node, rf);
}

void KuDesignerDoc::setPageHeaderAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasPageHeader *ph = new CanvasPageHeader(docCanvas->templ->props["LeftMargin"].first.toInt(),
		0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"].first.toInt() -
		docCanvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    ph->props["Height"].first = attributes.namedItem("Height").nodeValue();
    docCanvas->templ->pageHeader = ph;
    addReportItems(node, ph);
}

void KuDesignerDoc::setPageFooterAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasPageFooter *pf = new CanvasPageFooter(docCanvas->templ->props["LeftMargin"].first.toInt(),
		0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"].first.toInt() -
		docCanvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    pf->props["Height"].first = attributes.namedItem("Height").nodeValue();
    docCanvas->templ->pageFooter = pf;
    addReportItems(node, pf);
}

void KuDesignerDoc::setDetailHeaderAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasDetailHeader *dh = new CanvasDetailHeader(docCanvas->templ->props["LeftMargin"].first.toInt(),
		0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"].first.toInt() -
		docCanvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    dh->props["Level"].first = attributes.namedItem("Level").nodeValue();
    dh->props["Height"].first = attributes.namedItem("Height").nodeValue();
    docCanvas->templ->details[attributes.namedItem("Level").nodeValue().toInt()].first.first = dh;
    addReportItems(node, dh);
}

void KuDesignerDoc::setDetailAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasDetail *d = new CanvasDetail(docCanvas->templ->props["LeftMargin"].first.toInt(),
		0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"].first.toInt() -
		docCanvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    d->props["Level"].first = attributes.namedItem("Level").nodeValue();
    d->props["Height"].first = attributes.namedItem("Height").nodeValue();
    docCanvas->templ->details[attributes.namedItem("Level").nodeValue().toInt()].second = d;
    addReportItems(node, d);
}

void KuDesignerDoc::setDetailFooterAttributes(QDomNode *node)
{
    QDomNamedNodeMap attributes = node->attributes();

    CanvasDetailFooter *df = new CanvasDetailFooter(docCanvas->templ->props["LeftMargin"].first.toInt(),
		0, docCanvas->templ->width() - docCanvas->templ->props["RightMargin"].first.toInt() -
		docCanvas->templ->props["LeftMargin"].first.toInt(),
		attributes.namedItem("Height").nodeValue().toInt(), docCanvas);
    df->props["Level"].first = attributes.namedItem("Level").nodeValue();
    df->props["Height"].first = attributes.namedItem("Height").nodeValue();
    docCanvas->templ->details[attributes.namedItem("Level").nodeValue().toInt()].first.second = df;
    addReportItems(node, df);
}


bool KuDesignerDoc::openDocument(const KURL& url, const char *format /*=0*/)
{
  QString tmpfile;
  KIO::NetAccess::download( url, tmpfile );
  /////////////////////////////////////////////////
  // TODO: Add your document opening code here
  /////////////////////////////////////////////////

    QFile f(tmpfile);
    if (!f.open(IO_ReadOnly))
        return false;

    QDomDocument rt;
    if (!rt.setContent(&f))
    	return false;

    QDomNode report, rep;
    for (QDomNode report = rt.firstChild(); !report.isNull(); report = report.nextSibling())
    {
	if (report.nodeName() == "KugarTemplate")
	{
	    rep = report;
	    break;
	}
    }
    report = rep;

    QDomNamedNodeMap attributes = report.attributes();
    //getting the page width and height
    int height = 297;
    int width = 210;

    if (attributes.namedItem("PageOrientation").nodeValue().toInt())
    {
	int temp = height;
	height = width;
	width = temp;
    }

    //zooming canvas according to screen resolution
    QPrinter* printer;

    // Set the page size
    printer = new QPrinter();
    printer->setFullPage(true);
    printer->setPageSize((QPrinter::PageSize)attributes.namedItem("PageSize").nodeValue().toInt());
    printer->setOrientation((QPrinter::Orientation)attributes.namedItem("PageOrientation").nodeValue().toInt());

    // Get the page metrics and set appropriate wigth and height
    QPaintDeviceMetrics pdm(printer);
    width = pdm.width();
    height = pdm.height();

    //this is not needed anymore
    delete printer;

    //zooming canvas according to default zoom level - FitWidth;
//    int scale = this->width()/width;
//    width *= scale;
//    height *= scale;

    //creating canvas
    docCanvas = new MyCanvas(width, height);
    docCanvas->setAdvancePeriod(30);

    //creating KugarTemplate object
    CanvasKugarTemplate *templ = new CanvasKugarTemplate(0, 0, width, height, docCanvas);
    templ->show();
    templ->props["PageSize"].first = attributes.namedItem("PageSize").nodeValue();
    templ->props["PageOrientation"].first = attributes.namedItem("PageOrientation").nodeValue();
    templ->props["TopMargin"].first = attributes.namedItem("TopMargin").nodeValue();
    templ->props["BottomMargin"].first = attributes.namedItem("BottomMargin").nodeValue();
    templ->props["LeftMargin"].first = attributes.namedItem("LeftMargin").nodeValue();
    templ->props["RightMargin"].first = attributes.namedItem("RightMargin").nodeValue();

    //show canvas
    for (KuDesignerView *it = pViewList->first(); it; it = pViewList->next())
    {
        it->setCanvas(docCanvas);
    }

    // Get all the child report elements
    QDomNodeList children = report.childNodes();
    int childCount = children.length();
    for (int j = 0; j < childCount; j++)
    {
	QDomNode child = children.item(j);

	if (child.nodeType() == QDomNode::ElementNode)
	{
	    if (child.nodeName() == "ReportHeader")
		setReportHeaderAttributes(&child);
	    else if (child.nodeName() == "PageHeader")
		setPageHeaderAttributes(&child);
	    else if (child.nodeName() == "DetailHeader")
		setDetailHeaderAttributes(&child);
	    else if(child.nodeName() == "Detail")
		setDetailAttributes(&child);
	    else if(child.nodeName() == "DetailFooter")
		setDetailFooterAttributes(&child);
	    else if(child.nodeName() == "PageFooter")
		setPageFooterAttributes(&child);
	    else if(child.nodeName() == "ReportFooter")
		setReportFooterAttributes(&child);
	}
    }
    docCanvas->templ->arrangeSections(FALSE);
    QCanvasItemList l = docCanvas->allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); it++)
    {
	(*it)->show();
    }
    docCanvas->update();

    f.close();

  KIO::NetAccess::removeTempFile( tmpfile );

  modified=false;
  return true;
}

bool KuDesignerDoc::saveDocument(const KURL& url, const char *format /*=0*/)
{
  /////////////////////////////////////////////////
  // TODO: Add your document saving code here
  /////////////////////////////////////////////////
    KTempFile temp;

    QTextStream t(temp.file());
	t << docCanvas->templ->getXml();
    temp.close();

    KIO::NetAccess::upload(temp.name(), url);
    KIO::NetAccess::removeTempFile(temp.name());

    modified=false;
    return true;
}

void KuDesignerDoc::deleteContents()
{
  /////////////////////////////////////////////////
  // TODO: Add implementation to delete the document contents
  /////////////////////////////////////////////////
    if (docCanvas)
        delete docCanvas;
}

MyCanvas *KuDesignerDoc::canvas()
{
    return docCanvas;
}

void KuDesignerDoc::setCanvas(MyCanvas *c)
{
    if (c) docCanvas = c;
    return;
}
#include "kudesignerdoc.moc"
