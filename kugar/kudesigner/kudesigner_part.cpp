/* This file is part of the KDE project
   Copyright (C) 2002 Alexander Dymo <cloudtemple@mksat.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kudesigner_part.h"
#include "kudesigner_factory.h"
#include "kudesigner_view.h"
#include "canvbox.h"
#include "creportitem.h"
#include "ccalcfield.h"
#include "cfield.h"
#include "clabel.h"
#include "cline.h"
#include "cspecialfield.h"

#include <koTemplateChooseDia.h>

#include <klocale.h>
#include <kstandarddirs.h>

#include <qpainter.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qfileinfo.h>

#include "mycanvas.h"

KudesignerPart::KudesignerPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
    setInstance( KudesignerFactory::global(), false );
    history = new KCommandHistory(actionCollection());

    connect( history, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
    connect( history, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );
}

KudesignerPart::~KudesignerPart()
{
    delete history;
}

bool KudesignerPart::initDoc()
{
    // If nothing is loaded, do initialize here
    bool ok = FALSE;

    QString _template;

    KoTemplateChooseDia::ReturnType ret = KoTemplateChooseDia::choose(
        KudesignerFactory::global(), _template, "application/x-kudesigner", "*.kut", i18n("Kugar Designer"),
        KoTemplateChooseDia::Everything, "kudesigner_template");
    if ( ret == KoTemplateChooseDia::Template ) {
        QFileInfo fileInfo( _template );
        QString fileName( fileInfo.dirPath( TRUE ) + "/" + fileInfo.baseName() + ".ktm" );
        resetURL();
        ok = loadNativeFormat( fileName );
        setEmpty();
    } else if ( ret == KoTemplateChooseDia::File ) {
        KURL url( _template);
        ok = openURL( url );
    } else if ( ret == KoTemplateChooseDia::Empty ) {
        QString fileName( locate( "kudesinger_template", "General/.source/A4.ktm" , KudesignerFactory::global() ) );
        resetURL();
        ok = loadNativeFormat( fileName );
        setEmpty();
    }
    setModified( FALSE );

    return ok;
}

KoView* KudesignerPart::createViewInstance( QWidget* parent, const char* name )
{
    return new KudesignerView( this, parent, name );
}

bool KudesignerPart::loadXML( QIODevice *, const QDomDocument & rt)
{
    // TODO load the document from the QDomDocument

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
            {
                canvas()->templ->detailsCount++;
                setDetailAttributes(&child);
            }
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

    return true;
}

QDomDocument KudesignerPart::saveXML()
{
    // TODO save the document into a QDomDocument
    return QDomDocument();
}


void KudesignerPart::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/,
                                double /*zoomX*/, double /*zoomY*/ )
{
    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.

    canvas()->drawArea(rect, &painter, TRUE);

}

void KudesignerPart::commandExecuted()
{
    setModified(true);
}

void KudesignerPart::documentRestored()
{
    setModified(false);
}

MyCanvas *KudesignerPart::canvas()
{
    return docCanvas;
}

void KudesignerPart::setReportHeaderAttributes(QDomNode *node)
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

void KudesignerPart::setReportFooterAttributes(QDomNode *node)
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

void KudesignerPart::setPageHeaderAttributes(QDomNode *node)
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

void KudesignerPart::setPageFooterAttributes(QDomNode *node)
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

void KudesignerPart::setDetailHeaderAttributes(QDomNode *node)
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

void KudesignerPart::setDetailAttributes(QDomNode *node)
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

void KudesignerPart::setDetailFooterAttributes(QDomNode *node)
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


void KudesignerPart::addReportItems(QDomNode *node, CanvasBand *section)
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

void KudesignerPart::setReportItemAttributes(QDomNode *node, CanvasReportItem *item)
{
    QDomNamedNodeMap attributes = node->attributes();

    for (unsigned int i = 0; i < attributes.count(); i++)
    {
        item->props[attributes.item(i).nodeName()].first = attributes.item(i).nodeValue();
    }
}

#include "kudesigner_part.moc"
